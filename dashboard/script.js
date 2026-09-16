// ==========================================
// AEOLUS - DASHBOARD
// Dados simulados para testar a interface
// ==========================================


// Dados que futuramente virão do ESP32 pelo MQTT

let temperatura = 23.8;
let umidade = 64;
let luminosidade = 382;

let velocidade = 18.4;

let direcao = "Nordeste";
let angulo = 45;


// ==========================================
// ATUALIZA OS VALORES NA TELA
// ==========================================

function atualizarDashboard() {

    document.getElementById("temperatura").textContent =
        temperatura.toFixed(1);

    document.getElementById("umidade").textContent =
        umidade;

    document.getElementById("luminosidade").textContent =
        luminosidade;

    document.getElementById("velocidade").textContent =
        velocidade.toFixed(1);

    document.getElementById("direcao").textContent =
        direcao;

    document.getElementById("angulo").textContent =
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
        document.getElementById("ponteiro");

    ponteiro.style.transform =
        `translate(-50%, -100%) rotate(${angulo}deg)`;
}


// ==========================================
// CLASSIFICA A VELOCIDADE DO VENTO
// ==========================================

function classificarVento() {

    const nivel =
        document.getElementById("nivel-vento");

    const descricao =
        document.getElementById("descricao-vento");


    if (velocidade <= 10) {

        nivel.textContent = "Aura";

        descricao.textContent =
            "Vento suave";

    } else if (velocidade <= 25) {

        nivel.textContent = "Zéfiro";

        descricao.textContent =
            "Brisa moderada";

    } else if (velocidade <= 40) {

        nivel.textContent = "Noto";

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

    const agora = new Date();

    const horario =
        agora.toLocaleTimeString("pt-BR");

    document.getElementById(
        "ultima-atualizacao"
    ).textContent = horario;
}


// ==========================================
// INICIA A DASHBOARD
// ==========================================

atualizarDashboard();