Aeolus — Estação Meteorológica IoT
Projeto desenvolvido no Programa de Estágio em IoT do Vortex Lab — Universidade de Fortaleza (UNIFOR). A estação utiliza um ESP32 para coletar dados do ambiente, exibi-los na própria caixa e enviá-los a uma dashboard por MQTT.
O nome Aeolus faz referência a Éolo, figura da mitologia grega associada aos ventos. Essa identidade aparece na caixa, na abertura do display e na classificação visual da intensidade do vento.
Estado atual
Em 7 de outubro de 2026, os componentes foram testados individualmente na montagem e reunidos no firmware principal. O funcionamento geral da integração foi confirmado durante os testes, e as alterações foram incorporadas à develop e depois à main.
O backend, o PostgreSQL e o histórico já haviam funcionado em etapas anteriores. A gravação no banco e a exibição do histórico ainda precisam ser conferidas com a versão integrada atual. A leitura do LDR e a calibração física dos encoders também possuem limitações descritas neste documento.
Objetivo
Monitorar temperatura, umidade relativa do ar, resposta do sensor de luz, velocidade e direção do vento. As leituras são apresentadas em três frentes:
- LCD 16x2: consulta local dos dados e do relógio.
- LED RGB: indicação visual da faixa de velocidade do vento.
- Dashboard: leituras recebidas por MQTT e gráfico de histórico consultado pela API.
O projeto reúne sistemas embarcados, eletrônica, comunicação IoT, desenvolvimento web, banco de dados e fabricação de peças e PCB.
Tecnologias e componentes
Área	Tecnologias e componentes
Controle	ESP32 DevKit V4, C/C++, Arduino e PlatformIO
Sensores	DHT22, módulo LDR e dois encoders rotativos KY-040
Interface local	LCD 16x2 I2C, LED RGB e dois botões frontais
Comunicação	Wi-Fi, MQTT, JSON e PubSubClient
Backend	Node.js, Express, MQTT.js e PostgreSQL
Dashboard	HTML, CSS, JavaScript, MQTT.js e Chart.js
Fabricação	Peças impressas em 3D, PCB artesanal e componentes soldados
Desenvolvimento	VS Code, Git, GitHub e KiCad


O firmware utiliza as bibliotecas DHT sensor library e LiquidCrystal_I2C. As dependências do firmware e do backend são declaradas, respectivamente, em platformio.ini e backend/package.json.
Pinagem final
Placa: ESP32 DevKit V4.
Componente	Sinal	GPIO	Função
DHT22	DATA	4	Temperatura e umidade
LDR	Saída do módulo	35	Leitura bruta com analogRead
RGB	Vermelho	26	PWM do canal vermelho
RGB	Verde	25	PWM do canal verde
RGB	Azul	33	PWM do canal azul
LCD I2C	SDA	13	Dados; endereço 0x27
LCD I2C	SCL	14	Clock
Anemômetro/cata-vento	CLK	16	Canal de quadratura
Anemômetro/cata-vento	DT	17	Canal de quadratura
Anemômetro/cata-vento	SW	19	Referência do rotor para o cálculo
Biruta	CLK	21	Canal de quadratura
Biruta	DT	27	Canal adicionado durante os testes finais
Biruta	SW	22	Calibração da referência Norte
Botão frontal direito	Sinal	32	Liga/desliga somente o LCD
Botão frontal esquerdo	Sinal	23	Troca manualmente a página do LCD


O GPIO18 não é utilizado na montagem atual. O LDR está alimentado em 3,3 V. Os botões são lidos com INPUT_PULLUP, com acionamento em nível baixo.
RGB: o teste físico confirmou LOW para desligar e HIGH para acender. O PWM atual não é invertido. A indicação anterior de lógica common-anode invertida não corresponde ao comportamento verificado na montagem.
Funcionamento dos componentes
Temperatura e umidade
O DHT22 fornece temperatura em graus Celsius e umidade relativa em porcentagem. No firmware integrado, a leitura dos sensores ocorre a cada 2,5 segundos.
O módulo trata falhas de leitura e retorna -1 quando recebe um valor inválido. Esse retorno deve ser interpretado como erro conforme o contexto: uma temperatura real de −1 °C também é possível, portanto o valor sozinho não é um indicador universal de falha.
Luminosidade
O módulo atual lê o GPIO35 com analogRead, em resolução de 12 bits, e apresenta o resultado bruto entre 0 e 4095.
Nos testes, a iluminação produziu valores próximos de 0 e o sensor completamente coberto produziu 4095. A transição ficou abrupta, sem uma resposta gradual confiável. O módulo observado possui saída DO, o que explica a resposta próxima aos extremos mesmo quando essa tensão é lida pelo ADC.
Por decisão da equipe, a leitura bruta foi mantida nesta versão. Ela não representa lux nem comprova uma medição analógica proporcional à luminosidade. Para obter uma resposta gradual, será necessário revisar o circuito ou utilizar uma saída analógica apropriada e realizar a calibração.
Velocidade do vento
O anemômetro utiliza os canais CLK16 e DT17 para acompanhar o movimento por quadratura. A implementação preserva a lógica da Marianna: a velocidade é estimada pelo deslocamento e pelo tempo de passagem entre setores do rotor.
O cálculo utiliza raio de 0,042 m e uma hipótese de 80 passos por volta. As estimativas recentes por setor são combinadas; após aproximadamente três segundos sem movimento válido, a indicação retorna a zero.
O botão SW19 define a referência do rotor utilizada pelo cálculo. Nos testes manuais, foram observadas leituras de velocidade durante o giro e retorno a zero após a parada. Esses testes confirmam a resposta funcional, mas não a precisão da velocidade em relação a um instrumento de referência.
Direção do vento
A biruta utiliza CLK21 e DT27. Com os dois canais, é possível identificar o sentido de rotação: na montagem testada, o giro horário aumenta a contagem e o anti-horário diminui.
Para calibrar, aponte a biruta para o Norte e pressione o botão do próprio encoder, SW22. A posição passa a ser a referência de 0°. O firmware calcula o ângulo relativo e o classifica em oito direções: Norte, Nordeste, Leste, Sudeste, Sul, Sudoeste, Oeste e Noroeste.
O código considera 80 passos por volta, quantidade que ainda precisa ser confirmada com uma volta física completa. Como a leitura é incremental, a referência deve ser restabelecida após reiniciar ou reposicionar o conjunto. O botão define uma referência informada pela pessoa que calibra; o sensor não identifica o Norte geográfico sozinho.
Display e botões
O LCD é inicializado com Wire.begin(13, 14) no endereço 0x27. A abertura exibe uma animação temática com o nome AEOLUS.
As páginas apresentam:
1. Velocidade e direção do vento.
2. Temperatura, umidade e leitura bruta do LDR.
3. Hora e data.
O botão frontal direito, GPIO32, liga ou desliga somente o display. Os sensores e a comunicação continuam funcionando. O botão frontal esquerdo, GPIO23, troca as páginas manualmente; não há alternância automática nesta versão.
O relógio usa a data e a hora de compilação como referência. Ele não possui sincronização NTP nesta implementação.
RGB e Escala Aeolus
A Escala Aeolus é uma classificação visual própria do projeto. Os limites atuais permitem acompanhar a resposta do protótipo durante os testes e não correspondem a uma escala meteorológica padronizada.
Faixa nominal de velocidade	Classificação	Cor
Até 0,1 km/h	Parado	Desligado
Acima de 0,1 até 0,5 km/h	Aura	Azul
Acima de 0,5 até 1,0 km/h	Zéfiro	Ciano
Acima de 1,0 até 3,0 km/h	Noto	Verde
Acima de 3,0 km/h	Fúria de Bóreas	Vermelho


O código utiliza histerese de 10%: a mudança de faixa depende também do estado anterior, evitando trocas frequentes quando a leitura está próxima de um limite. As cores amarela e laranja foram retiradas após os testes físicos.
A dashboard utiliza as mesmas cores, limites e regra de histerese. Como recebe amostras por MQTT, pode haver diferença momentânea entre sua classificação e a cor local. Para garantir correspondência exata, uma melhoria possível é publicar também o estado escolhido pelo firmware.
Arquitetura e comunicação
O ESP32 lê os sensores e atualiza LCD e RGB. Pela rede Wi-Fi, publica as medições no broker MQTT. A dashboard recebe essas mensagens diretamente para atualizar os cartões e a bússola.
Em paralelo, o backend recebe as mensagens MQTT e armazena as leituras no PostgreSQL. A API REST disponibiliza os registros para o gráfico histórico. Isso significa que os cartões em tempo real podem funcionar mesmo quando o histórico está indisponível.
Serviço	Configuração utilizada no desenvolvimento
Broker MQTT	broker.hivemq.com
Tópico	aeolus/sensor/dados
MQTT da dashboard	wss://broker.hivemq.com:8884/mqtt
API local	http://localhost:3000
Histórico	GET /api/leituras
Estado do backend/banco	GET /api/status, conforme a documentação do backend


Exemplo ilustrativo do formato de mensagem documentado no projeto:
{
  "temperatura": 24.5,
  "umidade": 57.1,
  "luminosidade": 4095,
  "velocidade": 0.96,
  "angulo": 45,
  "direcao": "Nordeste",
  "statusDHT": true,
  "statusWiFi": true,
  "statusMQTT": true
}
A dashboard trata valores inválidos e ausência de mensagens recentes, evitando apresentar zero como substituto de um dado ausente. O gráfico consulta a API a cada cinco segundos e utiliza até 30 leituras recentes.
Banco de dados
O backend foi desenvolvido para registrar as mensagens na tabela leituras_aeolus. A estrutura documentada é:
Campo	Tipo
id	BIGINT, chave primária automática
temperatura	NUMERIC(5,2)
umidade	NUMERIC(5,2)
luminosidade	INTEGER
velocidade	NUMERIC(6,2)
angulo	INTEGER
direcao	VARCHAR(20)
status_dht	BOOLEAN
criado_em	TIMESTAMPTZ


Os campos de estado do Wi-Fi e do MQTT não fazem parte dessa estrutura documentada. A instalação do PostgreSQL e a criação da tabela são pré-requisitos para a persistência.
A integração anterior já utilizou esse fluxo. Na revisão atual, ainda é necessário conferir a gravação de novas mensagens, a resposta da API e a exibição dos horários e valores no gráfico. O funcionamento da dashboard em tempo real não comprova, sozinho, a persistência no banco.
Organização do repositório
Caminho	Responsabilidade
src/main.cpp	Inicialização e coordenação dos módulos
src/sensores/	DHT22, LDR, anemômetro e biruta
src/atuadores/	Controle do RGB
src/display/	LCD, animação e botões frontais
src/comunicacao/	Wi-Fi e MQTT
include/	Interfaces dos módulos e configuração local
backend/consumidor-mqtt.js	Recebimento das mensagens MQTT
backend/db.js	Conexão com PostgreSQL
backend/server.js	Servidor da API
backend/teste-banco.js e backend/teste-mqtt.js	Verificações auxiliares
dashboard/index.html	Estrutura da dashboard
dashboard/style.css	Aparência e responsividade
dashboard/script.js	MQTT, atualização da interface e histórico
testes-hardware/	Códigos de testes isolados
platformio.ini	Configuração da placa e dependências do firmware


Os módulos do firmware seguem o padrão .h para declarações e .cpp para implementação. O PlatformIO compila todos os arquivos .cpp dentro de src; por isso, os testes isolados ficam fora dessa pasta e precisam usar nomes exclusivos quando compartilham a compilação com os módulos.
Configuração e execução
Pré-requisitos
- VS Code com a extensão PlatformIO e suporte à placa ESP32.
- Cabo USB de dados e acesso à porta serial da placa.
- Node.js e npm para o backend.
- PostgreSQL em execução, banco configurado e tabela criada.
- Rede Wi-Fi disponível para o ESP32 e acesso ao broker MQTT.
1. Obter o projeto
git clone https://github.com/brenalemos09/aeolus-iot.git
cd aeolus-iot
2. Configurar o Wi-Fi
Crie include/credenciais.h com os dados da rede local. Exemplo com valores fictícios:
#ifndef CREDENCIAIS_H
#define CREDENCIAIS_H

#define WIFI_SSID "nome_da_rede"
#define WIFI_SENHA "senha_da_rede"

#endif
Esse arquivo deve permanecer fora do versionamento, assim como backend/.env, backend/node_modules/ e os arquivos de compilação. Não inclua credenciais reais em commits ou exemplos públicos.
3. Compilar e enviar o firmware
Abra a raiz do projeto no VS Code. Use Build para compilar e Upload para enviar à placa pelo PlatformIO. Abra o Serial Monitor em 115200 baud para acompanhar as leituras.
Como alternativa, no terminal com o comando pio disponível, execute na raiz:
pio run
pio run --target upload
pio device monitor --baud 115200
Após iniciar, calibre o anemômetro com SW19. Aponte a biruta para o Norte e pressione SW22. Teste os botões frontais separadamente dos botões dos encoders.
4. Preparar e iniciar o backend
Em outro terminal, a partir da raiz do projeto:
cd backend
npm install
Crie backend/.env com as configurações utilizadas pelo backend para acesso ao PostgreSQL e demais serviços. Consulte db.js, server.js e consumidor-mqtt.js para confirmar os nomes das variáveis esperadas; não basta criar um arquivo .env vazio.
Antes de iniciar, confirme que o banco e a tabela estão preparados. Os comandos de criação do banco e os nomes exatos das variáveis de ambiente ainda precisam ser consolidados em um guia reproduzível de instalação.
Inicie o servidor conforme o ponto de entrada documentado:
node server.js
Na validação do backend, confira também como consumidor-mqtt.js é iniciado: se ele não for carregado pelo servidor, precisará de um processo separado. A API estar online não comprova que o consumidor MQTT esteja gravando mensagens.
5. Abrir a dashboard
Sirva dashboard/index.html com um servidor local, como o Live Server do VS Code. O ESP32 precisa publicar no tópico configurado para atualizar as leituras em tempo real.
Para o gráfico histórico, o backend precisa estar disponível no endereço configurado em dashboard/script.js. O endereço localhost:3000 se refere ao computador que abre a página; para consultar a API a partir de outro dispositivo, ajuste esse endereço para o servidor correspondente.
Testes e validação
Os testes foram conduzidos por componente: código mínimo, leitura no Serial Monitor, acionamento físico e integração após confirmar a resposta.
Item	Situação em 07/10/2026
DHT22	Leitura isolada confirmada
LDR	Leitura bruta confirmada; resposta gradual não validada
Anemômetro	Movimento, cálculo de velocidade e SW19 testados
Biruta	Dois sentidos, calibração SW22, ângulo e direção testados
RGB	Canais e cores azul, ciano, verde e vermelho testados
LCD	Endereço I2C, texto, animação e controle local testados
Firmware integrado	Funcionamento geral confirmado pela equipe
Dashboard	Interface e lógica atualizadas; recebimento em tempo real relatado no projeto
Banco, API e histórico	Funcionaram anteriormente; revisão da versão atual pendente
Git	Integração enviada à main, sem alterações locais pendentes naquele momento


Os resultados de bancada comprovam o comportamento funcional observado. Ainda não constituem uma calibração meteorológica ou um teste prolongado de estabilidade.
Limitações e próximos passos
- Conferir o fluxo MQTT → backend → PostgreSQL → histórico da dashboard com a versão atual.
- Verificar datas, horários, ordem e valores dos registros exibidos no gráfico.
- Confirmar os passos por volta dos encoders e validar a velocidade com uma referência física.
- Revisar o circuito do LDR para obter resposta gradual, caso exigida pelo projeto.
- Consolidar as instruções de criação do banco e configuração do backend.
- Avaliar a estabilidade do sistema e a reconexão de Wi-Fi e MQTT em testes prolongados.
Versionamento
O desenvolvimento utiliza branches de funcionalidade, integração em develop e publicação da versão consolidada em main.
Em 07/10/2026, foi concluído o fluxo feature/integracao-final → develop → main. O commit da integração é 3f8dc16, e o merge final na main é bbcedcc. O envio ao GitHub foi confirmado com a árvore de trabalho limpa.
Identidade e aprendizados
A caixa recebeu o nome O Odre de Aeolus, e a dashboard, Templo de Aeolus. As peças do anemômetro, da biruta e da caixa foram desenvolvidas por impressão 3D, junto à construção da PCB artesanal.
Durante o desenvolvimento, a equipe enfrentou reinicializações ao iniciar o Wi-Fi nas placas utilizadas anteriormente, tentativas de fabricação da PCB e falhas de impressão 3D. A troca para a DevKit V4 permitiu avançar com os testes de comunicação. A causa elétrica exata das reinicializações não foi comprovada por essa troca.
Os testes finais também mostraram a importância de conferir a montagem antes de ajustar o código: a pinagem dos encoders havia mudado, a biruta precisava do segundo canal para acompanhar o sentido de rotação e o RGB respondia com polaridade diferente da documentada inicialmente.
Outro aprendizado foi separar leitura funcional de medição calibrada. Detectar movimento ou receber um número no ADC é uma etapa; demonstrar que esse valor representa corretamente uma grandeza física exige validação adicional.
Desenvolvimento
Projeto desenvolvido no Vortex Lab — UNIFOR, como parte do Programa de Estágio em IoT. A integração final foi realizada por Brena, em colaboração com Marianna, preservando a lógica original dos encoders e ajustando os módulos conforme os testes da montagem.
