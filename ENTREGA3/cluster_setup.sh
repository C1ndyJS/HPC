#!/bin/bash

# cluster_setup.sh - Script para configurar cluster MPI en AWS
# Autor: Cindy Marcela Jimenez Saldarriaga
# Fecha: Mayo-Junio 2025

# Colores para output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Función para logging con colores
log() {
    echo -e "${BLUE}[$(date '+%Y-%m-%d %H:%M:%S')]${NC} $1"
}

error() {
    echo -e "${RED}[ERROR]${NC} $1"
}

success() {
    echo -e "${GREEN}[SUCCESS]${NC} $1"
}

warning() {
    echo -e "${YELLOW}[WARNING]${NC} $1"
}

# Variables de configuración
HEAD_NODE_IP="172.31.81.100"
NFS_SHARE="/shared"
MPI_VERSION="4.1.7"
USER_HOME="/home/ec2-user"

# Detectar tipo de nodo
detect_node_type() {
    local current_ip=$(hostname -I | awk '{print $1}')
    
    if [ "$current_ip" == "$HEAD_NODE_IP" ]; then
        NODE_TYPE="head"
    else
        NODE_TYPE="worker"
    fi
    
    log "Detected node type: $NODE_TYPE (IP: $current_ip)"
}

# Actualizar sistema
update_system() {
    log "Updating system packages..."
    sudo yum update -y
    
    if [ $? -eq 0 ]; then
        success "System updated successfully"
    else
        error "Failed to update system"
        exit 1
    fi
}

# Instalar herramientas de desarrollo
install_development_tools() {
    log "Installing development tools..."
    
    sudo yum groupinstall -y "Development Tools"
    sudo yum install -y \
        gcc \
        gcc-c++ \
        make \
        cmake \
        git \
        wget \
        curl \
        vim \
        htop \
        bc \
        nfs-utils \
        rpcbind
    
    if [ $? -eq 0 ]; then
        success "Development tools installed"
    else
        error "Failed to install development tools"
        exit 1
    fi
}

# Instalar OpenMPI
install_openmpi() {
    log "Installing OpenMPI..."
    
    # Instalar OpenMPI desde repositorios
    sudo yum install -y openmpi openmpi-devel
    
    # Configurar variables de entorno
    echo 'export PATH=$PATH:/usr/lib64/openmpi/bin' >> ~/.bashrc
    echo 'export LD_LIBRARY_PATH=/usr/lib64/openmpi/lib:$LD_LIBRARY_PATH' >> ~/.bashrc
    echo 'export MANPATH=/usr/share/man/openmpi:$MANPATH' >> ~/.bashrc
    
    # Aplicar variables de entorno
    source ~/.bashrc
    
    # Verificar instalación
    /usr/lib64/openmpi/bin/mpicc --version
    
    if [ $? -eq 0 ]; then
        success "OpenMPI installed successfully"
    else
        error "Failed to install OpenMPI"
        exit 1
    fi
}

# Configurar NFS en nodo head
setup_nfs_server() {
    if [ "$NODE_TYPE" == "head" ]; then
        log "Setting up NFS server on head node..."
        
        # Crear directorio compartido
        sudo mkdir -p $NFS_SHARE
        sudo chown ec2-user:ec2-user $NFS_SHARE
        sudo chmod 755 $NFS_SHARE
        
        # Configurar exports
        echo "$NFS_SHARE *(rw,sync,no_root_squash,no_subtree_check)" | sudo tee -a /etc/exports
        
        # Iniciar servicios NFS
        sudo systemctl start rpcbind
        sudo systemctl enable rpcbind
        sudo systemctl start nfs-server
        sudo systemctl enable nfs-server
        
        # Exportar sistemas de archivos
        sudo exportfs -av
        
        success "NFS server configured"
    fi
}

# Configurar NFS en nodos worker
setup_nfs_client() {
    if [ "$NODE_TYPE" == "worker" ]; then
        log "Setting up NFS client on worker node..."
        
        # Crear punto de montaje
        sudo mkdir -p $NFS_SHARE
        
        # Montar sistema de archivos NFS
        sudo mount -t nfs $HEAD_NODE_IP:$NFS_SHARE $NFS_SHARE
        
        # Configurar montaje automático
        echo "$HEAD_NODE_IP:$NFS_SHARE $NFS_SHARE nfs defaults 0 0" | sudo tee -a /etc/fstab
        
        # Verificar montaje
        if mount | grep -q $NFS_SHARE; then
            success "NFS client configured and mounted"
        else
            error "Failed to mount NFS share"
            exit 1
        fi
    fi
}

# Configurar SSH sin contraseña
setup_ssh_keys() {
    log "Setting up passwordless SSH..."
    
    # Generar claves SSH si no existen
    if [ ! -f ~/.ssh/id_rsa ]; then
        ssh-keygen -t rsa -N "" -f ~/.ssh/id_rsa
        success "SSH keys generated"
    else
        log "SSH keys already exist"
    fi
    
    # Agregar clave pública a authorized_keys
    cat ~/.ssh/id_rsa.pub >> ~/.ssh/authorized_keys
    chmod 600 ~/.ssh/authorized_keys
    chmod 700 ~/.ssh
    
    # Configurar SSH para evitar verificación de host
    cat << EOF > ~/.ssh/config
Host *
    StrictHostKeyChecking no
    UserKnownHostsFile /dev/null
    LogLevel ERROR
EOF
    
    chmod 600 ~/.ssh/config
    
    success "SSH configuration completed"
}

# Crear archivo de hosts para MPI
create_hostfile() {
    if [ "$NODE_TYPE" == "head" ]; then
        log "Creating MPI hostfile..."
        
        cat << EOF > $NFS_SHARE/hosts.txt
172.31.81.100 slots=8
172.31.87.220 slots=8
172.31.86.226 slots=8
172.31.90.24 slots=8
EOF
        
        # Crear también en directorio local
        cp $NFS_SHARE/hosts.txt ~/hosts.txt
        
        success "Hostfile created"
    fi
}

# Configurar variables de entorno MPI
setup_mpi_environment() {
    log "Configuring MPI environment..."
    
    cat << EOF >> ~/.bashrc

# MPI Configuration
export OMPI_MCA_btl_tcp_if_include=eth0
export OMPI_MCA_btl_base_warn_component_unused=0
export OMPI_MCA_mpi_warn_on_fork=0
export OMPI_MCA_btl="^openib"

# Path configuration
export PATH=/usr/lib64/openmpi/bin:\$PATH
export LD_LIBRARY_PATH=/usr/lib64/openmpi/lib:\$LD_LIBRARY_PATH
EOF
    
    source ~/.bashrc
    success "MPI environment configured"
}

# Crear script de compilación
create_compile_script() {
    if [ "$NODE_TYPE" == "head" ]; then
        log "Creating compilation script..."
        
        cat << 'EOF' > $NFS_SHARE/compile.sh
#!/bin/bash

echo "Compiling matrix multiplication programs..."

# Compilar versión secuencial
echo "Compiling sequential version..."
gcc -O3 -o matrix_sequential matrix_sequential.c -lm
if [ $? -eq 0 ]; then
    echo "Sequential version compiled successfully"
else
    echo "ERROR: Failed to compile sequential version"
    exit 1
fi

# Compilar versión MPI
echo "Compiling MPI version..."
/usr/lib64/openmpi/bin/mpicc -O3 -o matrix_mpi matrix_mpi.c -lm
if [ $? -eq 0 ]; then
    echo "MPI version compiled successfully"
else
    echo "ERROR: Failed to compile MPI version"
    exit 1
fi

echo "Compilation completed successfully!"
EOF
        
        chmod +x $NFS_SHARE/compile.sh
        success "Compilation script created"
    fi
}

# Verificar instalación
verify_installation() {
    log "Verifying installation..."
    
    # Verificar OpenMPI
    if /usr/lib64/openmpi/bin/mpicc --version > /dev/null 2>&1; then
        success "OpenMPI is working"
    else
        error "OpenMPI verification failed"
    fi
    
    # Verificar NFS
    if [ "$NODE_TYPE" == "head" ]; then
        if systemctl is-active --quiet nfs-server; then
            success "NFS server is running"
        else
            error "NFS server is not running"
        fi
    else
        if mount | grep -q $NFS_SHARE; then
            success "NFS share is mounted"
        else
            error "NFS share is not mounted"
        fi
    fi
    
    # Verificar SSH
    if ssh -o BatchMode=yes -o ConnectTimeout=5 localhost echo "SSH test" > /dev/null 2>&1; then
        success "SSH passwordless access is working"
    else
        warning "SSH passwordless access needs manual setup"
    fi
}

# Mostrar información del sistema
show_system_info() {
    log "System Information:"
    echo "===================="
    echo "Hostname: $(hostname)"
    echo "IP Address: $(hostname -I | awk '{print $1}')"
    echo "Node Type: $NODE_TYPE"
    echo "OS: $(cat /etc/os-release | grep PRETTY_NAME | cut -d'"' -f2)"
    echo "OpenMPI Version: $(/usr/lib64/openmpi/bin/mpicc --version | head -n 1)"
    echo "NFS Share: $NFS_SHARE"
    echo "User Home: $USER_HOME"
    echo "===================="
}