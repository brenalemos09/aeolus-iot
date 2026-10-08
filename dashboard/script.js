const broker = "wss://broker.hivemq.com:8884/mqtt";
const topico = "aeolus/sensor/dados";
const urlHistorico = "http://localhost:3000/api/leituras";

const limiteOffline = 7000;
const limiteHistorico = 30;

const dadosAtuais = {
    temperatura: null,
    umidade: null,
    luminosidade: null,
    velocidade: null,
    direcao: null,
    angulo: null,
    statusDHT: false
};

const faixasVento = [
    {
        nome: "Parado",
        descricao: "LED apagado",
        chave: "parado",
        cor: "#94a3b8"
    },
    {
        nome: "Aura",
        descricao: "LED azul",
        chave: "aura",
        cor: "#2563eb"
    },
    {
        nome: "Zéfiro",
        descricao: "LED ciano",
        chave: "zefiro",
        cor: "#0e7490"
    },
    {
        nome: "Noto",
        descricao: "LED verde",
        chave: "noto",
        cor: "#096e2e"
    },
    {
        nome: "Fúria de Bóreas",
        descricao: "LED vermelho",
        chave: "boreas",
        cor: "#dc2626"
    }
];

const limitesVento = [0.1, 0.5, 1.0, 3.0];
const histereseVento = 0.10;

let faixaAtual = 0;
let ultimaMensagem = 0;
let dadosExpirados = false;
let carregandoHistorico = false;
let rotacaoPonteiro = null;

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
    statusHistorico: document.getElementById("status-historico"),
    cardVento: document.querySelector(".vento-card")
};

let grafico = null;

if (typeof Chart !== "undefined") {
    grafico = new Chart(
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
                        tension: 0.3,
                        spanGaps: false
                    },
                    {
                        label: "Umidade (%)",
                        data: historicoUmidade,
                        borderColor: "#3b82f6",
                        backgroundColor: "rgba(59, 130, 246, 0.08)",
                        borderWidth: 2,
                        pointRadius: 2,
                        pointHoverRadius: 5,
                        tension: 0.3,
                        spanGaps: false
                    },
                    {
                        label: "Vento (km/h)",
                        data: historicoVento,
                        borderColor: "#06b6d4",
                        backgroundColor: "rgba(6, 182, 212, 0.08)",
                        borderWidth: 2,
                        pointRadius: 2,
                        pointHoverRadius: 5,
                        tension: 0.3,
                        spanGaps: false,
                        yAxisID: "vento"
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
                        grid: {
                            color: "#edf0f4"
                        },
                        ticks: {
                            color: "#6b7280"
                        }
                    },
                    vento: {
                        position: "right",
                        beginAtZero: true,
                        title: {
                            display: true,
                            text: "Vento (km/h)",
                            color: "#6b7280"
                        },
                        grid: {
                            drawOnChartArea: false
                        },
                        ticks: {
                            color: "#6b7280"
                        }
                    }
                }
            }
        }
    );
} else {
    elementos.statusHistorico.textContent =
        "Biblioteca do gráfico indisponível";
}

function numeroValido(valor) {
    if (
        valor === null ||
        valor === undefined ||
        typeof valor === "boolean"
    ) {
        return null;
    }

    if (typeof valor !== "number" && typeof valor !== "string") {
        return null;
    }

    if (typeof valor === "string" && valor.trim() === "") {
        return null;
    }

    const numero = Number(valor);
    return Number.isFinite(numero) ? numero : null;
}

function booleanoValido(valor) {
    if (valor === true || valor === 1) {
        return true;
    }

    if (valor === false || valor === 0) {
        return false;
    }

    if (typeof valor === "string") {
        const texto = valor.trim().toLowerCase();

        if (texto === "true" || texto === "1") {
            return true;
        }

        if (texto === "false" || texto === "0") {
            return false;
        }
    }

    return null;
}

function numeroNoIntervalo(valor, minimo, maximo) {
    const numero = numeroValido(valor);

    if (
        numero === null ||
        numero < minimo ||
        numero > maximo
    ) {
        return null;
    }

    return numero;
}

function lerDadosDHT(dados) {
    const temperatura = numeroNoIntervalo(
        dados.temperatura,
        -40,
        80
    );

    const umidade = numeroNoIntervalo(
        dados.umidade,
        0,
        100
    );

    const statusInformado = booleanoValido(
        dados.statusDHT ?? dados.status_dht
    );

    let valido =
        statusInformado !== false &&
        temperatura !== null &&
        umidade !== null;

    // Sem status informado, -1 representa erro no módulo atual.
    if (statusInformado === null && temperatura === -1) {
        valido = false;
    }

    return {
        valido,
        temperatura: valido ? temperatura : null,
        umidade: valido ? umidade : null
    };
}

function mostrarNumero(elemento, valor, casas = 1) {
    elemento.textContent =
        valor === null ? "--" : valor.toFixed(casas);
}

function atualizarStatus(texto, tipo) {
    elementos.statusTexto.textContent = texto;
    elementos.statusPonto.className = `status-ponto ${tipo}`;
}

function atualizarBussola(angulo) {
    if (angulo === null) {
        elementos.ponteiro.style.visibility = "hidden";
        rotacaoPonteiro = null;
        return;
    }

    elementos.ponteiro.style.visibility = "visible";

    if (rotacaoPonteiro === null) {
        rotacaoPonteiro = angulo;
    } else {
        const anteriorNormalizado =
            ((rotacaoPonteiro % 360) + 360) % 360;

        // Evita uma volta inteira ao cruzar entre 359° e 0°.
        const diferenca =
            ((angulo - anteriorNormalizado + 540) % 360) - 180;

        rotacaoPonteiro += diferenca;
    }

    elementos.ponteiro.style.transform =
        `translate(-50%, -100%) rotate(${rotacaoPonteiro}deg)`;
}

function obterClassificacao(velocidade) {
    if (velocidade === null || velocidade < 0) {
        return {
            nome: "Aguardando",
            descricao: dadosExpirados
                ? "Sem leitura recente"
                : "Sem leitura válida",
            chave: "",
            cor: "#6b7280"
        };
    }

    while (
        faixaAtual < faixasVento.length - 1 &&
        velocidade > limitesVento[faixaAtual] * (1 + histereseVento)
    ) {
        faixaAtual++;
    }

    while (
        faixaAtual > 0 &&
        velocidade < limitesVento[faixaAtual - 1] * (1 - histereseVento)
    ) {
        faixaAtual--;
    }

    return faixasVento[faixaAtual];
}

function atualizarClassificacao(velocidade) {
    const classificacao = obterClassificacao(velocidade);

    elementos.nivelVento.textContent = classificacao.nome;
    elementos.nivelVento.style.color = classificacao.cor;
    elementos.descricaoVento.textContent = classificacao.descricao;

    if (classificacao.chave) {
        elementos.cardVento.dataset.nivel = classificacao.chave;
    } else {
        delete elementos.cardVento.dataset.nivel;
    }

    document.querySelectorAll(".nivel").forEach(function (nivel) {
        nivel.classList.toggle(
            "ativo",
            nivel.dataset.nivel === classificacao.chave
        );
    });
}

function atualizarInterface() {
    const recente = !dadosExpirados;

    const temperatura = recente ? dadosAtuais.temperatura : null;
    const umidade = recente ? dadosAtuais.umidade : null;
    const luminosidade = recente ? dadosAtuais.luminosidade : null;
    const velocidade = recente ? dadosAtuais.velocidade : null;
    const angulo = recente ? dadosAtuais.angulo : null;
    const direcao = recente ? dadosAtuais.direcao : null;

    mostrarNumero(elementos.temperatura, temperatura);
    mostrarNumero(elementos.umidade, umidade);
    mostrarNumero(elementos.velocidade, velocidade, 2);

    elementos.luminosidade.textContent =
        luminosidade === null ? "--" : Math.round(luminosidade);

    elementos.direcao.textContent = direcao || "--";

    elementos.angulo.textContent =
        angulo === null ? "--" : Math.round(angulo);

    atualizarBussola(angulo);
    atualizarClassificacao(velocidade);
}

function receberDados(mensagem) {
    const dados = JSON.parse(mensagem.toString());

    if (
        dados === null ||
        typeof dados !== "object" ||
        Array.isArray(dados)
    ) {
        throw new Error("O payload deve ser um objeto JSON.");
    }

    const camposEsperados = [
        "temperatura",
        "umidade",
        "luminosidade",
        "velocidade",
        "angulo",
        "direcao"
    ];

    if (!camposEsperados.some(campo =>
        Object.prototype.hasOwnProperty.call(dados, campo)
    )) {
        throw new Error("Payload sem os campos esperados do Aeolus.");
    }

    const dht = lerDadosDHT(dados);

    dadosAtuais.statusDHT = dht.valido;
    dadosAtuais.temperatura = dht.temperatura;
    dadosAtuais.umidade = dht.umidade;

    // O ESP32 já envia a luminosidade convertida.
    dadosAtuais.luminosidade = numeroNoIntervalo(
        dados.luminosidade,
        0,
        120000
    );

    const velocidade = numeroValido(dados.velocidade);

    dadosAtuais.velocidade =
        velocidade !== null && velocidade >= 0
            ? velocidade
            : null;

    const angulo = numeroValido(dados.angulo);

    dadosAtuais.angulo =
        angulo === null ? null : ((angulo % 360) + 360) % 360;

    dadosAtuais.direcao =
        typeof dados.direcao === "string" && dados.direcao.trim()
            ? dados.direcao.trim()
            : null;

    ultimaMensagem = Date.now();
    dadosExpirados = false;

    atualizarStatus(
        dht.valido
            ? "Recebendo dados"
            : "Recebendo dados · DHT sem leitura válida",
        "online"
    );

    elementos.ultimaAtualizacao.textContent =
        new Date(ultimaMensagem).toLocaleTimeString("pt-BR");

    atualizarInterface();
}

if (typeof mqtt !== "undefined") {
    const clienteMQTT = mqtt.connect(broker);

    clienteMQTT.on("connect", function () {
        atualizarStatus("MQTT conectado · aguardando dados", "aguardando");

        clienteMQTT.subscribe(topico, function (erro) {
            if (erro) {
                atualizarStatus("Erro ao assinar o tópico", "offline");
                console.error("Erro na assinatura MQTT:", erro.message);
            }
        });
    });

    clienteMQTT.on("message", function (topicoRecebido, mensagem) {
        if (topicoRecebido !== topico) {
            return;
        }

        try {
            receberDados(mensagem);
        } catch (erro) {
            console.error("Mensagem MQTT inválida:", erro.message);
        }
    });

    clienteMQTT.on("reconnect", function () {
        atualizarStatus("Reconectando ao MQTT", "aguardando");
    });

    clienteMQTT.on("offline", function () {
        atualizarStatus("MQTT desconectado", "offline");
    });

    clienteMQTT.on("close", function () {
        atualizarStatus("MQTT desconectado", "offline");
    });

    clienteMQTT.on("error", function (erro) {
        console.error("Erro MQTT:", erro.message);
        atualizarStatus("Erro na conexão MQTT", "offline");
    });
} else {
    atualizarStatus("Biblioteca MQTT indisponível", "offline");
}

async function carregarHistorico() {
    if (carregandoHistorico || !grafico) {
        return;
    }

    carregandoHistorico = true;

    try {
        const resposta = await fetch(urlHistorico, {
            cache: "no-store",
            signal: AbortSignal.timeout(8000)
        });

        if (!resposta.ok) {
            throw new Error(`API indisponível: HTTP ${resposta.status}`);
        }

        const resultado = await resposta.json();

        const leituras = Array.isArray(resultado)
            ? resultado
            : resultado?.value;

        if (!Array.isArray(leituras)) {
            throw new Error("Formato inesperado do histórico.");
        }

        const ultimasLeituras = leituras
            .filter(leitura =>
                leitura !== null &&
                typeof leitura === "object" &&
                leitura.criado_em &&
                Number.isFinite(Date.parse(leitura.criado_em))
            )
            .sort((a, b) =>
                Date.parse(a.criado_em) - Date.parse(b.criado_em)
            )
            .slice(-limiteHistorico);

        horarios.length = 0;
        historicoTemperatura.length = 0;
        historicoUmidade.length = 0;
        historicoVento.length = 0;

        ultimasLeituras.forEach(function (leitura) {
            horarios.push(
                new Date(leitura.criado_em).toLocaleTimeString(
                    "pt-BR",
                    {
                        hour: "2-digit",
                        minute: "2-digit",
                        second: "2-digit"
                    }
                )
            );

            const dht = lerDadosDHT(leitura);

            historicoTemperatura.push(dht.temperatura);
            historicoUmidade.push(dht.umidade);

            const velocidade = numeroValido(leitura.velocidade);

            historicoVento.push(
                velocidade !== null && velocidade >= 0
                    ? velocidade
                    : null
            );
        });

        grafico.update();

        elementos.statusHistorico.textContent =
            ultimasLeituras.length > 0
                ? `${ultimasLeituras.length} leituras carregadas`
                : "Nenhuma leitura registrada";
    } catch (erro) {
        elementos.statusHistorico.textContent =
            horarios.length > 0
                ? "Sem atualização · último histórico mantido"
                : "Histórico indisponível";

        console.error("Erro ao carregar histórico:", erro.message);
    } finally {
        carregandoHistorico = false;
    }
}

setInterval(function () {
    if (ultimaMensagem === 0 || dadosExpirados) {
        return;
    }

    if (Date.now() - ultimaMensagem > limiteOffline) {
        dadosExpirados = true;
        atualizarStatus("Sem dados recentes", "offline");
        atualizarInterface();
    }
}, 1000);

atualizarInterface();
carregarHistorico();

setInterval(carregarHistorico, 5000);