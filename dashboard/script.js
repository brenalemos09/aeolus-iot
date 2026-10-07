const dadosAtuais = {
    temperatura: null,
    umidade: null,
    luminosidade: null,
    velocidade: null,
    direcao: "Norte",
    angulo: 0,
    statusDHT: false
};

const broker = "wss://broker.hivemq.com:8884/mqtt";
const topico = "aeolus/sensor/dados";
const limiteOffline = 7000;
const limiteHistorico = 30;

let ultimaMensagem = 0;

const horarios = [];
const historicoTemperatura = [];
const historicoUmidade = [];
const historicoVento = [];

const elementos = {
    temperatura: document.getElementById("temperatura"),
    umidade: document.getElementById("umidade"),
    luminosidade: document.getElementById("luminosidade"),
    velocidade: document.getElementById("velocidade"),
    direcao: document.getElementById("direcao"),
    angulo: document.getElementById("angulo"),
    ponteiro: document.getElementById("ponteiro"),
    nivelVento: document.getElementById("nivel-vento"),
    descricaoVento: document.getElementById("descricao-vento"),
    statusTexto: document.getElementById("status-texto"),
    statusPonto: document.getElementById("status-ponto"),
    ultimaAtualizacao: document.getElementById("ultima-atualizacao"),
    statusHistorico: document.getElementById("status-historico")
};

const grafico = new Chart(
    document.getElementById("grafico-condicoes"),
    {
        type: "line",

        data: {
            labels: horarios,

            datasets: [
                {
                    label: "Temperatura (°C)",
                    data: historicoTemperatura,
                    borderColor: "#f59e0b",
                    backgroundColor: "rgba(245, 158, 11, 0.08)",
                    borderWidth: 2,
                    pointRadius: 2,
                    pointHoverRadius: 5,
                    tension: 0.3
                },
                {
                    label: "Umidade (%)",
                    data: historicoUmidade,
                    borderColor: "#3b82f6",
                    backgroundColor: "rgba(59, 130, 246, 0.08)",
                    borderWidth: 2,
                    pointRadius: 2,
                    pointHoverRadius: 5,
                    tension: 0.3
                },
                {
                    label: "Vento (km/h)",
                    data: historicoVento,
                    borderColor: "#0ea5e9",
                    backgroundColor: "rgba(14, 165, 233, 0.08)",
                    borderWidth: 2,
                    pointRadius: 2,
                    pointHoverRadius: 5,
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
                    position: "top",
                    align: "end",

                    labels: {
                        usePointStyle: true,
                        boxWidth: 8,
                        color: "#4b5563",

                        font: {
                            size: 11
                        }
                    }
                }
            },

            scales: {
                x: {
                    grid: {
                        display: false
                    },

                    ticks: {
                        color: "#6b7280",
                        maxTicksLimit: 8
                    }
                },

                y: {
                    beginAtZero: true,

                    grid: {
                        color: "#edf0f4"
                    },

                    ticks: {
                        color: "#6b7280"
                    }
                }
            }
        }
    }
);

function numeroValido(valor) {
    const numero = Number(valor);

    if (Number.isFinite(numero)) {
        return numero;
    }

    return null;
}

function mostrarNumero(elemento, valor, casas = 1) {
    if (valor === null) {
        elemento.textContent = "--";
        return;
    }

    elemento.textContent = valor.toFixed(casas);
}

function atualizarStatus(texto, tipo) {
    elementos.statusTexto.textContent = texto;
    elementos.statusPonto.className = `status-ponto ${tipo}`;
}

function atualizarInterface() {
    mostrarNumero(
        elementos.temperatura,
        dadosAtuais.temperatura
    );

    mostrarNumero(
        elementos.umidade,
        dadosAtuais.umidade
    );

    mostrarNumero(
        elementos.velocidade,
        dadosAtuais.velocidade
    );

    elementos.luminosidade.textContent =
        dadosAtuais.luminosidade ?? "--";

    elementos.direcao.textContent =
        dadosAtuais.direcao || "--";

    elementos.angulo.textContent =
        Math.round(dadosAtuais.angulo);

    atualizarBussola();
    atualizarClassificacao();
}

function atualizarBussola() {
    const anguloNormalizado =
        ((dadosAtuais.angulo % 360) + 360) % 360;

    elementos.ponteiro.style.transform =
        `translate(-50%, -100%) rotate(${anguloNormalizado}deg)`;
}

function obterClassificacao(velocidade) {
    if (velocidade === null) {
        return {
            nome: "Aguardando",
            descricao: "Sem leitura recente",
            chave: ""
        };
    }

    if (velocidade <= 10) {
        return {
            nome: "Aura",
            descricao: "Vento suave",
            chave: "aura"
        };
    }

    if (velocidade <= 25) {
        return {
            nome: "Zéfiro",
            descricao: "Brisa moderada",
            chave: "zefiro"
        };
    }

    if (velocidade <= 40) {
        return {
            nome: "Noto",
            descricao: "Vento intenso",
            chave: "noto"
        };
    }

    return {
        nome: "Fúria de Bóreas",
        descricao: "Vento muito intenso",
        chave: "boreas"
    };
}

function atualizarClassificacao() {
    const classificacao =
        obterClassificacao(dadosAtuais.velocidade);

    elementos.nivelVento.textContent =
        classificacao.nome;

    elementos.descricaoVento.textContent =
        classificacao.descricao;

    document
        .querySelectorAll(".nivel")
        .forEach(function (nivel) {
            nivel.classList.toggle(
                "ativo",
                nivel.dataset.nivel === classificacao.chave
            );
        });
}

function registrarAtualizacao() {
    elementos.ultimaAtualizacao.textContent =
        new Date().toLocaleTimeString("pt-BR");
}

function receberDados(mensagem) {
    const dados =
        JSON.parse(mensagem.toString());

    dadosAtuais.statusDHT =
        Boolean(dados.statusDHT);

    if (dadosAtuais.statusDHT) {
        dadosAtuais.temperatura =
            numeroValido(dados.temperatura);

        dadosAtuais.umidade =
            numeroValido(dados.umidade);
    } else {
        dadosAtuais.temperatura = null;
        dadosAtuais.umidade = null;
    }

    dadosAtuais.luminosidade =
        numeroValido(dados.luminosidade);

    dadosAtuais.velocidade =
        numeroValido(dados.velocidade);

    dadosAtuais.angulo =
        numeroValido(dados.angulo) ?? 0;

    dadosAtuais.direcao =
        dados.direcao || "--";

    ultimaMensagem = Date.now();

    atualizarStatus(
        "Recebendo dados",
        "online"
    );

    registrarAtualizacao();
    atualizarInterface();
}

const clienteMQTT =
    mqtt.connect(broker);

clienteMQTT.on(
    "connect",
    function () {
        atualizarStatus(
            "MQTT conectado",
            "aguardando"
        );

        clienteMQTT.subscribe(
            topico,
            function (erro) {
                if (erro) {
                    atualizarStatus(
                        "Erro ao assinar o tópico",
                        "offline"
                    );
                }
            }
        );
    }
);

clienteMQTT.on(
    "message",
    function (topicoRecebido, mensagem) {
        if (topicoRecebido !== topico) {
            return;
        }

        try {
            receberDados(mensagem);
        } catch (erro) {
            console.error(
                "Mensagem MQTT inválida:",
                erro.message
            );
        }
    }
);

clienteMQTT.on(
    "reconnect",
    function () {
        atualizarStatus(
            "Reconectando ao MQTT",
            "aguardando"
        );
    }
);

clienteMQTT.on(
    "offline",
    function () {
        atualizarStatus(
            "MQTT desconectado",
            "offline"
        );
    }
);

clienteMQTT.on(
    "error",
    function (erro) {
        console.error(
            "Erro MQTT:",
            erro.message
        );

        atualizarStatus(
            "Erro na conexão MQTT",
            "offline"
        );
    }
);

async function carregarHistorico() {
    try {
        const resposta =
            await fetch(
                "http://localhost:3000/api/leituras"
            );

        if (!resposta.ok) {
            throw new Error(
                "API indisponível"
            );
        }

        const resultado =
            await resposta.json();

        const leituras =
            Array.isArray(resultado)
                ? resultado
                : resultado.value || [];

        const ultimasLeituras =
            leituras.slice(-limiteHistorico);

        horarios.length = 0;
        historicoTemperatura.length = 0;
        historicoUmidade.length = 0;
        historicoVento.length = 0;

        ultimasLeituras.forEach(
            function (leitura) {
                const horario =
                    new Date(
                        leitura.criado_em
                    ).toLocaleTimeString(
                        "pt-BR",
                        {
                            hour: "2-digit",
                            minute: "2-digit",
                            second: "2-digit"
                        }
                    );

                horarios.push(horario);

                historicoTemperatura.push(
                    numeroValido(
                        leitura.temperatura
                    )
                );

                historicoUmidade.push(
                    numeroValido(
                        leitura.umidade
                    )
                );

                historicoVento.push(
                    numeroValido(
                        leitura.velocidade
                    )
                );
            }
        );

        grafico.update();

        elementos.statusHistorico.textContent =
            `${ultimasLeituras.length} leituras carregadas`;
    } catch (erro) {
        elementos.statusHistorico.textContent =
            "Histórico indisponível";

        console.error(
            "Erro ao carregar histórico:",
            erro.message
        );
    }
}

setInterval(
    function () {
        if (ultimaMensagem === 0) {
            return;
        }

        const tempoSemDados =
            Date.now() - ultimaMensagem;

        if (tempoSemDados > limiteOffline) {
            atualizarStatus(
                "Sem dados recentes",
                "offline"
            );
        }
    },
    1000
);

atualizarInterface();
carregarHistorico();

setInterval(
    carregarHistorico,
    5000
);