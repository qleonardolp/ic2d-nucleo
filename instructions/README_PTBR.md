# ForceCAST Nucleo Framework - AltairLab | LegRo

Este repositório é dedicado ao desenvolvimento do software embarcado na placa STM32 NUCLEO-F446RE com a bancada de testes IC2D usando a interface gráfica ForceCAST Studio. As instruções a seguir estão segmentadas de acordo com o uso do software/bancada, sendo indicado seguir estritamente ao conjunto de instruções que se aplica a cada caso. Antes de prosseguir é necessário que o usuário esteja ciente das instruções de uso dos outros componentes da bancada, tal como o acionamento hidráulico e o atuador elétrico LinMot ([Manual IC2D](TODO_link_to_manual)).

## Usuário básico: executando experimentos sem alterações no código 

As instruções a seguir devem ser seguidas para o uso básico da bancada com o código base, sem alterações.

## Usuário intermediário: executando experimentos com alterações no código

As instruções a seguir devem ser seguidas por todos os usuários que consideram contribuir com o código fonte ou desenvolver algoritmos de controle. A não aderência às instruções implicará em rejeição de _pull requests_ (PR) automática. Recomenda-se a leitura das instruções do [usuário básico](#usuário-básico-executando-experimentos-sem-alterações-no-código) também, uma vez que o procedimento experimental padrão deve ser mantido. Alterações no código que impliquem em mudança no procedimento padrão, tais como na arquitetura da interface gráfica ou local de salvamento dos dados experimentais, deve ser indicado na PR ou considerado nas alterações de [usurário avançado](#usuário-avançado-entendendo-as-dependências-do-framework-e-fazendo-alterações-nelas).

Visando trabalhar com uma abordagem mundialmente conhecida em projetos de _software_ aberto, o procedimento adotado aqui é o Git Flow padrão. Resumidamente, aos familiarizados com git, cada usuário deve fazer um _fork_  deste repositório com sua conta GitHub e clonar o seu _fork_, **com um nome adequado**, na seguinte pasta do computador da bancada:

```bash
cd /home/ic2d/legged_ws/docker/container/nucle2-legged/home/ic2d/
git clone https://github.com/<seu_usuario>/ic2d-nucleo.git <your_folder_name>
```

Para compilar o seu código, execute o seguinte comando na sua pasta:
```bash
pio run -e ic2d-release
```

Para enviar o seu código compilado para a placa, conecte a placa pelo cabo USB e execute o seguinte comando na sua pasta:
```bash
pio run -e ic2d-release -t upload
```

Após ter uma versão estável do seu código (testada e validada) você pode submeter a PR pelo GitHub para que suas alterações sejam incluídas nesse repositório.

### Criando um controlador novo

Confira se o controle que você precisa já existe [aqui](/lib/forecastnucleoframework/src/controllers/). Os caminhos de pasta descritos nessa seção são relativos a pasta raiz do seu repositório. Crie novos arquivos de cabeçalho e código fonte (.hpp e .cpp) nas respectivas pastas:

```bash
lib/forecastnucleoframework/include/forecast/controllers
```

```bash
lib/forecastnucleoframework/src/controllers
```

Seu novo controlador precisa ser incluído no arquivo [src/main.cpp](/src/main.cpp):

```
/** Controllers Headers */
#include <forecast/controllers/PositionPID.hpp>
//...
#include <forecast/controllers/your_new_controller.hpp>
```

E dentro da função __main__:
```
app.get_controller_factory().add("Nome do Controlador", make_nome_controle_builder());
```

## Usuário avançado: entendendo as dependências do framework e fazendo alterações nelas

Primeiramente, espero que você saiba o que esta fazendo. Segundamente, boa sorte! Esse framework baseado em Platformio foi originalmente desenvolvido com:
- Plataforma STM32 versão 5.3.0;
- Framework Mbed versão [5.51105.220603](https://github.com/qleonardolp/framework-mbed-5.51105.220603). Essa versão não está mais disponível pela [distribuição oficial Platformio](https://registry.platformio.org/tools/platformio/framework-mbed);
- Instalação do Platformio com Python >3.6 e <3.9.