# Forecast Nucleo Framework - Legged Robotics Group (LegRo)

Este repositório é dedicado ao desenvolvimento do software embarcado na placa STM32 NUCLEO-F446RE com a bancada de testes IC2D usando a interface gráfica ForceCAST Studio. Inicialmente esse repositório se destina a ser o ambiente de desenvolvimento dedicado à IC2D, sem interação com os repositórios originais do **_AltairLab_**, apesar de herdar o histórico git original. Para acessar o repositório orginal, [clique aqui](https://gitlab.com/altairLab/elasticteam/ForecastNucleoFramework-test/-/tree/NEXT/).

Outro objetivo desse repositório é servir de base para o versionamento do código fonte, enquanto utilizamos uma imagem Docker para facilitar o uso para diversos usuários. É recomendada experiência prévia mínina com _git_ e _linux_ para configuração desse ambiente de desenvolvimento.

## Configuração

1. Em sua máquina é necessário instalar o _git_ e, recomendado, o editor [Visual Studio Code](https://code.visualstudio.com/):

    ```bash
    sudo apt install git 
    sudo snap install code --classic
    ```

2. Instale o Docker em seu computador seguindo as instruções desse [repositório](https://github.com/lomcin/linux-stuffs#docker);

3. Gere a imagem e inicialize o _container_ 'nucleo-legged' segundo instruções desse [repositório](https://github.com/lomcin/legged_ws).

## Instalação do programa ForeCAST NEXT

1. Baixar o arquivo 'Forecast NEXT-0.0.1.AppImage' ( [link](https://drive.google.com/file/d/1iX5ELgxvNkgTH1Kr4nGauSGmoDLqYUBu/view?usp=share_link) )

2. No terminal, execute os seguintes comandos:

    ```bash
    chmod u+x Forecast\ NEXT-0.0.1.AppImage
    sudo apt install libfuse2
    ```

3. Configure o acesso do PlatformIO ao usb do computador segundo essas [instruções](https://docs.platformio.org/en/latest/core/installation/udev-rules.html).

## Observações

A configuração deste repositório git, assim como a adoção da imagem docker, auxilia o desenvolvimento do software automatizando algumas configurações prévias e evitando erros provenientes de instalações locais (em cada computador). Aqui não é necessário resovler a instalação do [PlatformIO](https://docs.platformio.org/en/latest/what-is-platformio.html), muito menos a versão compatível do Python (3.6+ até 3.9!). Isso já está configurado na imagem docker. Quanto configuração original do repositório, tal como descrita [aqui](https://gitlab.com/altairLab/elasticteam/ForecastNucleoFramework-test/-/tree/NEXT/#get-the-firmware), também já está resolvida nesse novo repositório.

A única diferença é a necessidade de utilzarmos o PlatformIO pela linha de comando (CLI), já que não será feito uso da extensão no VS Code. A seguir estão as instruções para realizar compilação e envio para placa com o PlatformIO.

**Nota: caso precise clonar este repositório separadamente use:**
```bash
git clone --recurse-submodules git@github.com:qleonardolp/forecast-nucleo.git
```

## Compilação e Envio: PlatformIO CLI

WIP...

1. Compilando:
    ```bash
    pio run -e <nome_do_ambiente>
    ```

2. Compilando e enviando para placa:
    ```bash
    pio run -e <nome_do_ambiente> -t upload
    ```

3. Limpando o projeto:
    ```bash
    pio run -t clean
    ```

## Configuração VS Code

Para facilitar o desenvolvimento com o VS Code, algumas configurações foram necessárias ([ver arquivo](.vscode/c_cpp_properties.json)):

1. Adicionado o caminho dos cabeçalhos do framework Mbed ao _includePath_:
    ```
    "includePath": [
                "${workspaceFolder}/**",
                "${HOME}/.platformio/packages/framework-mbed/**"
            ]
    ```

2. Adicionado definições para facilitar a visualização do código:

    ```
    "defines": [
                "TARGET_STM32F4",
                "DEVICE_ANALOGOUT",
                "IC2D_SETUP"
            ]
    ```

## Documentação (Doxygen)

Executando a geração da documentação (comando na raiz do repositório):
```bash
doxygen Doxyfile
```