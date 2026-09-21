const banco = require("./db");

async function testarConexao() {
    try {
        const resultado = await banco.query("SELECT NOW() AS horario");

        console.log("Conexão com PostgreSQL realizada com sucesso!");
        console.log("Horário do banco:", resultado.rows[0].horario);
    } catch (erro) {
        console.error("Erro ao conectar ao PostgreSQL:", erro.message);
    } finally {
        await banco.end();
    }
}

testarConexao();