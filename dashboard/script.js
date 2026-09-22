// ==========================================
// AEOLUS - DASHBOARD
// VORTEX LAB
// ==========================================


// ==========================================
// DADOS RECEBIDOS DA ESP32
// ==========================================

let temperatura = 0;
let umidade = 0;
let luminosidade = 0;

let velocidade = 0;

let direcao = "Norte";
let angulo = 0;


// Guarda quando chegou a última mensagem da ESP32
let ultimaMensagemRecebida = 0;


// Se ficar mais de 7 segundos sem receber dados,
// o sistema será considerado offline
const tempoLimiteOffline = 7000;


// ==========================================
// HISTÓRICO DO GRÁFICO
// ==========================================

const horarios = [];

const historicoTemperatura = [];
const historicoUmidade = [];
const historicoVento = [];

const limiteHistorico = 30;


// ==========================================
// CONFIGURAÇÃO MQTT
// ==========================================

const broker =
    "wss://broker.hivemq.com:8884/mqtt";

const topico =
    "aeolus/sensor/dados";


// Cria a conexão com o broker MQTT
const clienteMQTT =
    mqtt.connect(broker);


// ==========================================
// GRÁFICO DE MONITORAMENTO
// ==========================================

const contextoGrafico =
    document.getElementById(
        "grafico-condicoes"
    );


const graficoCondicoes =
    new Chart(
        contextoGrafico,
        {
            type: "line",

            data: {

                labels: horarios,

                datasets: [

                    {
                        label: "Temperatura",
                        data: historicoTemperatura,

                        borderColor: "#b88732",
                        backgroundColor: "transparent",

                        borderWidth: 2,

                        pointRadius: 2,

                        tension: 0.3
                    },

                    {
                        label: "Umidade",
                        data: historicoUmidade,

                        borderColor: "#3982ba",
                        backgroundColor: "transparent",

                        borderWidth: 2,

                        pointRadius: 2,

                        tension: 0.3
                    },

                    {
                        label: "Vento",
                        data: historicoVento,

                        borderColor: "#d97832",
                        backgroundColor: "transparent",

                        borderWidth: 2,

                        pointRadius: 2,

                        tension: 0.3
                    }

                ]
            },

            options: {

                responsive: true,

                maintainAspectRatio: false,

                animation: false,

                interaction: {
                    intersect: false,
                    mode: "index"
                },

                plugins: {

                    legend: {
                        display: false
                    }
                },

                scales: {

                    x: {

                        grid: {
                            display: false
                        },

                        ticks: {
                            maxTicksLimit: 8
                        }
                    },

                    y: {

                        beginAtZero: true
                    }
                }
            }
        }
    );


// ==========================================
// CONEXÃO MQTT
// ==========================================

clienteMQTT.on(
    "connect",
    function () {

        console.log(
            "Dashboard conectada ao MQTT"
        );


        clienteMQTT.subscribe(
            topico,
            function (erro) {

                if (!erro) {

                    console.log(
                        "Inscrito no tópico:",
                        topico
                    );

                } else {

                    console.log(
                        "Erro ao assinar tópico MQTT"
                    );
                }
            }
        );
    }
);


// ==========================================
// RECEBE AS MENSAGENS DA ESP32
// ==========================================

clienteMQTT.on(
    "message",
    function (topicoRecebido, mensagem) {

        const texto =
            mensagem.toString();


        ultimaMensagemRecebida =
            Date.now();


        atualizarStatusSistema(true);


        console.log(
            "MQTT recebido:",
            texto
        );


        try {

            const dados =
                JSON.parse(texto);


            // ==================================
            // DADOS DOS SENSORES
            // ==================================

            temperatura =
                dados.temperatura;

            umidade =
                dados.umidade;

            luminosidade =
                dados.luminosidade;

            velocidade =
                dados.velocidade;

            angulo =
                dados.angulo;

            direcao =
                dados.direcao;


            // ==================================
            // STATUS DOS COMPONENTES
            // ==================================

            atualizarStatusDHT(
                dados.statusDHT
            );


            // ==================================
            // ATUALIZA A INTERFACE
            // ==================================

            atualizarDashboard();

            

        } catch (erro) {

            console.log(
                "Erro ao interpretar dados MQTT:",
                erro
            );
        }
    }
);


// ==========================================
// CASO A CONEXÃO MQTT CAIA
// ==========================================

clienteMQTT.on(
    "offline",
    function () {

        console.log(
            "Dashboard desconectada do MQTT"
        );

        atualizarStatusSistema(false);
    }
);


clienteMQTT.on(
    "error",
    function (erro) {

        console.log(
            "Erro MQTT:",
            erro
        );

        atualizarStatusSistema(false);
    }
);


// ==========================================
// STATUS GERAL DO SISTEMA
// ==========================================

function atualizarStatusSistema(online) {

    const texto =
        document.getElementById(
            "status-texto"
        );

    const ponto =
        document.getElementById(
            "status-ponto"
        );


    if (online) {

        texto.textContent =
            "Sistema online";

        ponto.style.background =
            "#42a878";

    } else {

        texto.textContent =
            "Sistema offline";

        ponto.style.background =
            "#b9433d";
    }
}


// ==========================================
// STATUS DO DHT22
// ==========================================

function atualizarStatusDHT(online) {

    const ponto =
        document.getElementById(
            "status-dht-ponto"
        );

    const texto =
        document.getElementById(
            "status-dht-texto"
        );


    if (!ponto || !texto) {
        return;
    }


    if (online) {

        ponto.style.background =
            "#42a878";

        texto.textContent =
            "Online";

        texto.style.color =
            "#42a878";

    } else {

        ponto.style.background =
            "#b9433d";

        texto.textContent =
            "Offline";

        texto.style.color =
            "#b9433d";
    }
}


// ==========================================
// ATUALIZA OS VALORES NA TELA
// ==========================================

function atualizarDashboard() {

    document.getElementById(
        "temperatura"
    ).textContent =
        Number(
            temperatura
        ).toFixed(1);


    document.getElementById(
        "umidade"
    ).textContent =
        Number(
            umidade
        ).toFixed(1);


    document.getElementById(
        "luminosidade"
    ).textContent =
        luminosidade;


    document.getElementById(
        "velocidade"
    ).textContent =
        Number(
            velocidade
        ).toFixed(1);


    document.getElementById(
        "direcao"
    ).textContent =
        direcao;


    document.getElementById(
        "angulo"
    ).textContent =
        angulo;


    atualizarBussola();

    classificarVento();

    atualizarHorario();
}


// ==========================================
// GIRA O PONTEIRO DA BÚSSOLA
// ==========================================

function atualizarBussola() {

    const ponteiro =
        document.getElementById(
            "ponteiro"
        );


    ponteiro.style.transform =
        `translate(-50%, -100%) rotate(${angulo}deg)`;
}


// ==========================================
// CLASSIFICA A VELOCIDADE DO VENTO
// ==========================================

function classificarVento() {

    const nivel =
        document.getElementById(
            "nivel-vento"
        );


    const descricao =
        document.getElementById(
            "descricao-vento"
        );


    if (velocidade <= 10) {

        nivel.textContent =
            "Aura";

        descricao.textContent =
            "Vento suave";

    } else if (velocidade <= 25) {

        nivel.textContent =
            "Zéfiro";

        descricao.textContent =
            "Brisa moderada";

    } else if (velocidade <= 40) {

        nivel.textContent =
            "Noto";

        descricao.textContent =
            "Vento intenso";

    } else {

        nivel.textContent =
            "Fúria de Bóreas";

        descricao.textContent =
            "Vento muito intenso";
    }
}


// ==========================================
// HORÁRIO DA ÚLTIMA ATUALIZAÇÃO
// ==========================================

function atualizarHorario() {

    const agora =
        new Date();


    const horario =
        agora.toLocaleTimeString(
            "pt-BR"
        );


    document.getElementById(
        "ultima-atualizacao"
    ).textContent =
        horario;
}


// ==========================================
// ATUALIZA O GRÁFICO
// ==========================================

// ==========================================
// CARREGA O HISTÓRICO DO POSTGRESQL
// ==========================================

async function carregarHistorico() {

    try {

        const resposta = await fetch(
            "http://localhost:3000/api/leituras"
        );

        if (!resposta.ok) {
            throw new Error(
                "Não foi possível consultar a API."
            );
        }

        const dados = await resposta.json();

        const leituras = Array.isArray(dados)
            ? dados
            : dados.value || [];

        const ultimasLeituras =
            leituras.slice(-limiteHistorico);

        horarios.length = 0;
        historicoTemperatura.length = 0;
        historicoUmidade.length = 0;
        historicoVento.length = 0;

        ultimasLeituras.forEach(
            function (leitura) {

                const data =
                    new Date(leitura.criado_em);

                const horario =
                    data.toLocaleTimeString(
                        "pt-BR",
                        {
                            hour: "2-digit",
                            minute: "2-digit",
                            second: "2-digit"
                        }
                    );

                horarios.push(horario);

                historicoTemperatura.push(
                    Number(leitura.temperatura)
                );

                historicoUmidade.push(
                    Number(leitura.umidade)
                );

                historicoVento.push(
                    Number(leitura.velocidade)
                );
            }
        );

        graficoCondicoes.update();

        console.log(
            "Histórico carregado da API:",
            ultimasLeituras.length,
            "leituras"
        );

    } catch (erro) {

        console.log(
            "Erro ao carregar histórico:",
            erro.message
        );
    }
}

// ==========================================
// VERIFICA SE A ESP32 PAROU DE ENVIAR DADOS
// ==========================================

setInterval(
    function () {

        const agora =
            Date.now();


        if (
            ultimaMensagemRecebida === 0 ||
            agora -
            ultimaMensagemRecebida >
            tempoLimiteOffline
        ) {

            atualizarStatusSistema(false);
        }

    },
    1000
);


// ==========================================
// ESTADO INICIAL DA DASHBOARD
// ==========================================

atualizarDashboard();

atualizarStatusSistema(false);

carregarHistorico();

setInterval(
    carregarHistorico,
    5000
);