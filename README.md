# ForceCAST Nucleo Framework - AltairLab | LegRo

Welcome to the ForceCAST Nucleo Framework developed by the [AltairLab](https://gitlab.com/altairLab/elasticteam/ForecastNucleoFramework-test/-/tree/NEXT/), and adapted by the Legged Robotics Group (LegRo) for the Impedance Control 2 Dimensions (IC2D) bench. The present tools are intented to work with the board STM32 NUCLEO-F446RE. In order to avoid issues regarding the Python version on the host machine it is strongly recommended to use our docker image. Detailed instructions follows in the links below.

- [Instructions in English](instructions/README_EN.md)
- [Instruções em Português](instructions/README_PTBR.md)

**If you consider to clone this repo directly, use this:**
```bash
git clone --recurse-submodules https://github.com/qleonardolp/ic2d-nucleo.git
```

## Build and upload to the board

1. Build only:
    ```bash
    pio run -e <env_name>
    ```

2. Build and upload:  
    ```bash
    pio run -e <env_name> -t upload  
    ```