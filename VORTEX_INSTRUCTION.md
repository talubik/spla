# Building SPLA to work with Vortex GPU

### First step

- Build vortex
```shell
python ./build_vortex.py
```

## Second step

- Run build with these arguments
```shell
python ./build.py [other options] --vortex=YES --vortex-dir=<path_to_vortex_dir> \
--vortex-tooldir=<path_to_vortex_tools> 
```

## Third step

- Configure vortex

```shell
eval "$(python ./configure_vortex.py --vortex-dir=<path_to_vortex_dir> [other options])"
```