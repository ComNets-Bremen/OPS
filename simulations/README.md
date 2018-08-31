# Simulation files

This simulation campaign was done to determine the initial confidence interval computations (using Student-t) for the following sets of simulations.

- Epidemic and Keetchi using random application with generation intervals 15s, 60s, 250s, 500s
- Epidemic and Keetchi using random mobility with number of nodes [250:250:2000]
- Epidemic and Keetchi using random mobility with cache sizes 500kB, 1MB, 3MB


# Simulation Configurations
## Reference Scenario
### Keetchi
- **herald-keetchi-random-mob.ini**: Simulation of the default scenario for Keetchi with 500 nodes and 5MB cache (30 runs)
- **herald-keetchi-random-mob-single-run.ini**: Simulation of the default scenario for Keetchi with 500 nodes and 5MB cache (single run)

### Epidemic
- **herald-epidemic-random-mob.ini**: Simulation of the default scenario for Epidemic routing with 500 nodes and 5MB cache (30 runs)
- **herald-epidemic-random-mob-single-run.ini**: Simulation of the default scenario for Epidemic routing with 500 nodes and 5MB cache (single run)

## Evaluation of the number of nodes
### Keetchi
- **herald-keetchi-random-mob-250-nodes.ini**: Simulation of the default scenario for Epidemic routing with the number of nodes set to 250 (30 runs)
- **herald-keetchi-random-mob-250-nodes-single-run.ini**: Simulation of the default scenario for Epidemic routing with the number of nodes set to 250 (single run)
- **herald-keetchi-random-mob-500-nodes.ini**: Simulation of the default scenario for Keetchi with the number of nodes set to 500 (30 runs)
- **herald-keetchi-random-mob-500-nodes-single-run.ini**: Simulation of the default scenario for Keetchi with the number of nodes set to 500 (single run)
- **herald-keetchi-random-mob-750-nodes.ini**: Simulation of the default scenario for Keetchi with the number of nodes set to 750 (30 runs)
- **herald-keetchi-random-mob-750-nodes-single-run.ini**: Simulation of the default scenario for Keetchi with the number of nodes set to 750 (single run)
- **herald-keetchi-random-mob-1000-nodes.ini**: Simulation of the default scenario for Keetchi with the number of nodes set to 1000 (30 runs)
- **herald-keetchi-random-mob-1000-nodes-single-run.ini**: Simulation of the default scenario for Keetchi with the number of nodes set to 1000 (single run)
- **herald-keetchi-random-mob-1250-nodes.ini**: Simulation of the default scenario for Keetchi with the number of nodes set to 1250 (30 runs)
- **herald-keetchi-random-mob-1250-nodes-single-run.ini**: Simulation of the default scenario for Keetchi with the number of nodes set to 1250 (single run)
- **herald-keetchi-random-mob-1500-nodes.ini**: Simulation of the default scenario for Keetchi with the number of nodes set to 1500 (30 runs)
- **herald-keetchi-random-mob-1500-nodes-single-run.ini**: Simulation of the default scenario for Keetchi with the number of nodes set to 1500 (single run)
- **herald-keetchi-random-mob-1750-nodes.ini**: Simulation of the default scenario for Keetchi with the number of nodes set to 1750 (30 runs)
- **herald-keetchi-random-mob-1750-nodes-single-run.ini**: Simulation of the default scenario for Keetchi with the number of nodes set to 1750 (single run)
- **herald-keetchi-random-mob-2000-nodes.ini**: Simulation of the default scenario for Keetchi with the number of nodes set to 2000 (30 runs)
- **herald-keetchi-random-mob-2000-nodes-single-run.ini**: Simulation of the default scenario for Keetchi with the number of nodes set to 2000 (single run)

### Epidemic
- **herald-epidemic-random-mob-250-nodes.ini**: Simulation of the default scenario for Epidemic routing with the number of nodes set to 250 (30 runs)
- **herald-epidemic-random-mob-250-nodes-single-run.ini**: Simulation of the default scenario for Epidemic routing with the number of nodes set to 250 (single run)
- **herald-epidemic-random-mob-500-nodes.ini**: Simulation of the default scenario for Epidemic routing with the number of nodes set to 500 (30 runs)
- **herald-epidemic-random-mob-500-nodes-single-run.ini**: Simulation of the default scenario for Epidemic routing with the number of nodes set to 500 (single run)
- **herald-epidemic-random-mob-750-nodes.ini**: Simulation of the default scenario for Epidemic routing with the number of nodes set to 750 (30 runs)
- **herald-epidemic-random-mob-750-nodes-single-run.ini**: Simulation of the default scenario for Epidemic routing with the number of nodes set to 750 (single run)
- **herald-epidemic-random-mob-1000-nodes.ini**: Simulation of the default scenario for Epidemic routing with the number of nodes set to 1000 (30 runs)
- **herald-epidemic-random-mob-1000-nodes-single-run.ini**: Simulation of the default scenario for Epidemic routing with the number of nodes set to 1000 (single run)
- **herald-epidemic-random-mob-1250-nodes.ini**: Simulation of the default scenario for Epidemic routing with the number of nodes set to 1250 (30 runs)
- **herald-epidemic-random-mob-1250-nodes-single-run.ini**: Simulation of the default scenario for Epidemic routing with the number of nodes set to 1250 (single run)
- **herald-epidemic-random-mob-1500-nodes.ini**: Simulation of the default scenario for Epidemic routing with the number of nodes set to 1500 (30 runs)
- **herald-epidemic-random-mob-1500-nodes-single-run.ini**: Simulation of the default scenario for Epidemic routing with the number of nodes set to 1500 (single run)
- **herald-epidemic-random-mob-1750-nodes.ini**: Simulation of the default scenario for Epidemic routing with the number of nodes set to 1750 (30 runs)
- **herald-epidemic-random-mob-1750-nodes-single-run.ini**: Simulation of the default scenario for Epidemic routing with the number of nodes set to 1750 (single run)
- **herald-epidemic-random-mob-2000-nodes.ini**: Simulation of the default scenario for Epidemic routing with the number of nodes set to 2000 (30 runs)
- **herald-epidemic-random-mob-2000-nodes-single-run.ini**: Simulation of the default scenario for Epidemic routing with the number of nodes set to 2000 (single run)

## Evaluation of the cache size
### Keetchi
- **herald-keetchi-random-mob-netpress-197.ini**: Simulation of the default scenario for Keetchi with the cache size set to 20KB (30 runs)
- **herald-keetchi-random-mob-netpress-197-single-run.ini**: Simulation of the default scenario for Keetchi with the cache size set to 20KB (single run)
- **herald-keetchi-random-mob-netpress-148.ini**: Simulation of the default scenario for Keetchi with the cache size set to 40KB (30 runs)
- **herald-keetchi-random-mob-netpress-148-single-run.ini**: Simulation of the default scenario for Keetchi with the cache size set to 40KB (single run)
- **herald-keetchi-random-mob-netpress-118.ini**: Simulation of the default scenario for Keetchi with the cache size set to 50KB (30 runs)
- **herald-keetchi-random-mob-netpress-118-single-run.ini**: Simulation of the default scenario for Keetchi with the cache size set to 50KB (single run)
- **herald-keetchi-random-mob-netpress-59-2.ini**: Simulation of the default scenario for Keetchi with the cache size set to 100KB (30 runs)
- **herald-keetchi-random-mob-netpress-59-2-single-run.ini**: Simulation of the default scenario for Keetchi with the cache size set to 100KB (single run)
- **herald-keetchi-random-mob-cache-500kB.ini**: Simulation of the default scenario for Keetchi with the cache size set to 500KB (30 runs)
- **herald-keetchi-random-mob-cache-500kB-single-run.ini**: Simulation of the default scenario for Keetchi with the cache size set to 500KB (single run)
- **herald-keetchi-random-mob-cache-1MB.ini**: Simulation of the default scenario for Keetchi with the cache size set to 1MB (30 runs)
- **herald-keetchi-random-mob-cache-1MB-single-run.ini**: Simulation of the default scenario for Keetchi with the cache size set to 1MB (single run)
- **herald-keetchi-random-mob-cache-3MB.ini**: Simulation of the default scenario for Keetchi with the cache size set to 3MB (30 runs)
- **herald-keetchi-random-mob-cache-3MB-single-run.ini**: Simulation of the default scenario for Keetchi with the cache size set to 3MB (single run)

### Epidemic
- **herald-epidemic-random-mob-netpress-197.ini**: Simulation of the default scenario for Epidemic routing with the cache size set to 20KB (30 runs)
- **herald-epidemic-random-mob-netpress-197-single-run.ini**: Simulation of the default scenario for Epidemic routing with the cache size set to 20KB (single run)
- **herald-epidemic-random-mob-netpress-148.ini**: Simulation of the default scenario for Epidemic routing with the cache size set to 40KB (30 runs)
- **herald-epidemic-random-mob-netpress-148-single-run.ini**: Simulation of the default scenario for Epidemic routing with the cache size set to 40KB (single run)
- **herald-epidemic-random-mob-netpress-118.ini**: Simulation of the default scenario for Epidemic routing with the cache size set to 50KB (30 runs)
- **herald-epidemic-random-mob-netpress-118-single-run.ini**: Simulation of the default scenario for Epidemic routing with the cache size set to 50KB (single run)
- **herald-epidemic-random-mob-netpress-59-2.ini**: Simulation of the default scenario for Epidemic routing with the cache size set to 100KB (30 runs)
- **herald-epidemic-random-mob-netpress-59-2-single-run.ini**: Simulation of the default scenario for Epidemic routing with the cache size set to 100KB (single run)
- **herald-epidemic-random-mob-cache-500kB.ini**: Simulation of the default scenario for Epidemic routing with the cache size set to 500KB (30 runs)
- **herald-epidemic-random-mob-cache-500kB-single-run.ini**: Simulation of the default scenario for Epidemic routing with the cache size set to 500KB (single run)
- **herald-epidemic-random-mob-cache-1MB.ini**: Simulation of the default scenario for Epidemic routing with the cache size set to 1MB (30 runs)
- **herald-epidemic-random-mob-cache-1MB-single-run.ini**: Simulation of the default scenario for Epidemic routing with the cache size set to 1MB (single run)
- **herald-epidemic-random-mob-cache-3MB.ini**: Simulation of the default scenario for Epidemic routing with the cache size set to 3MB (30 runs)
- **herald-epidemic-random-mob-cache-3MB-single-run.ini**: Simulation of the default scenario for Epidemic routing with the cache size set to 3MB (single run)
