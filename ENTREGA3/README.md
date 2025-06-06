# Matrix Multiplication with MPI on AWS Cluster

A high-performance computing implementation of parallel matrix multiplication using MPI (Message Passing Interface) deployed on Amazon Web Services.

## Overview

This project implements and evaluates parallel matrix multiplication algorithms using MPI across a distributed cluster. The implementation compares sequential and parallel approaches, analyzing performance metrics including speedup, efficiency, and scalability.

## Features

- **Parallel Matrix Multiplication**: MPI-based implementation with row-wise distribution
- **Performance Analysis**: Comprehensive benchmarking with multiple matrix sizes and process configurations
- **AWS Cluster Setup**: Automated cluster configuration scripts for EC2 instances
- **Scalability Testing**: Performance evaluation from 2 to 32 processes
- **Matrix Size Range**: Testing from small (10×10) to large matrices (3200×3200)

## Project Structure

```
├── cluster_setup.sh              # Automated cluster configuration script
├── matriz_secuencial_modified.c  # Sequential matrix multiplication implementation
├── matrix_mpi.c                  # MPI parallel matrix multiplication (referenced in docs)
├── compile.sh                    # Compilation script for both versions
├── run_experiments.sh            # Automated benchmarking script
├── hosts.txt                     # MPI hostfile configuration
└── results.csv                   # Performance results output
```

## Prerequisites

- AWS Account with EC2 access
- Amazon Linux 2023 instances
- OpenMPI 4.1.0 or later
- GCC compiler
- NFS utilities for shared storage

## Quick Start

### 1. Cluster Setup

Run the cluster setup script on each node:

```bash
chmod +x cluster_setup.sh
./cluster_setup.sh
```

The script automatically:
- Detects node type (head/worker)
- Installs development tools and OpenMPI
- Configures NFS shared storage
- Sets up passwordless SSH
- Creates MPI environment configuration

### 2. Compilation

Compile both sequential and parallel versions:

```bash
cd /shared  # or your shared directory
./compile.sh
```

### 3. Running Experiments

#### Single Test
```bash
# Sequential version
./matrix_sequential 1000

# MPI version (4 processes)
mpirun -np 4 -hostfile hosts.txt ./matrix_mpi 1000
```

#### Automated Benchmarking
```bash
./run_experiments.sh
```

## Configuration

### Cluster Architecture

The implementation uses a homogeneous cluster with:
- **Head Node**: Coordination and job submission
- **Worker Nodes**: Distributed computation
- **Shared Storage**: NFS for code and data sharing

### Default Configuration

- **Instance Type**: t2.micro (customizable)
- **Matrix Sizes**: 10, 100, 200, 400, 800, 1600, 3200
- **Process Counts**: 2, 4, 8, 16, 32
- **Network**: 10 Gbps within VPC

## Performance Metrics

The implementation measures:
- **Execution Time**: Wall-clock time for computation
- **Speedup**: Sequential time / Parallel time
- **Efficiency**: Speedup / Number of processes
- **Scalability**: Performance behavior with increasing processes

## Results Summary

Key findings from performance analysis:

- **Optimal Range**: Matrices larger than 400×400 show significant speedup
- **Best Configuration**: 8-16 processes for most use cases
- **Maximum Speedup**: Up to 28.7× with 32 processes on large matrices
- **Communication Overhead**: Becomes significant with small matrices or high process counts

## Code Examples

### Sequential Implementation
```c
void matrix_multiply_sequential(double* A, double* B, double* C, int n) {
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            C[i*n + j] = 0.0;
            for (int k = 0; k < n; k++) {
                C[i*n + j] += A[i*n + k] * B[k*n + j];
            }
        }
    }
}
```

### MPI Parallel Implementation
The MPI version distributes matrix rows across processes and uses collective communication for data distribution and result collection.

## AWS Deployment

### Instance Requirements
- **Minimum**: t2.micro instances
- **Recommended**: Compute-optimized instances (c5.large or higher)
- **Network**: Enhanced networking enabled
- **Storage**: EBS with adequate IOPS for data-intensive workloads

### Security Configuration
- SSH access (port 22)
- NFS traffic (port 2049)
- MPI communication (ports 10000-65535)

## Troubleshooting

### Common Issues

1. **SSH Connection Failed**
   ```bash
   # Regenerate SSH keys
   ssh-keygen -t rsa -N "" -f ~/.ssh/id_rsa
   cat ~/.ssh/id_rsa.pub >> ~/.ssh/authorized_keys
   ```

2. **NFS Mount Issues**
   ```bash
   # Restart NFS services
   sudo systemctl restart nfs-server
   sudo mount -a
   ```

3. **MPI Communication Errors**
   ```bash
   # Check network configuration
   export OMPI_MCA_btl_tcp_if_include=eth0
   ```

## Performance Optimization

### Recommendations
- Use matrices larger than 400×400 for effective parallelization
- Configure 8-16 processes for optimal balance
- Consider blocked algorithms for very large matrices
- Use compute-optimized instances for production workloads

### Advanced Optimizations
- Implement 2D matrix distribution
- Use BLAS-optimized kernels
- Overlap computation and communication
- Consider GPU acceleration for extremely large problems

## Contributing

1. Fork the repository
2. Create a feature branch
3. Make your changes
4. Test on actual cluster setup
5. Submit a pull request

## License

This project is available under the MIT License. See LICENSE file for details.

## References

- MPI Standard Documentation
- OpenMPI User Guide
- AWS EC2 Documentation
- High Performance Computing best practices

---

**Note**: This implementation is designed for educational and research purposes. For production HPC workloads, consider using optimized libraries like Intel MKL or OpenBLAS.