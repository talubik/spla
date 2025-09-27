# Building SPLA to work with Vortex GPU

### Prerequisites
- Vortex:
  [Vortex GPGPU GitHub](https://github.com/vortexgpgpu/vortex)
- Build Vortex

## First step

- Run build with these arguments
```shell
python ./build.py [other options] --vortex=YES --vortex-dir=<path_to_vortex_dir> \
--vortex-tooldir=<path_to_vortex_tools> 
```

## Second step

- Configure vortex

```shell
eval "$(python ./configure_vortex.py --vortex-dir=<path_to_vortex_dir> [other options])"
```