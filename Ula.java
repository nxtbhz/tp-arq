import java.io.File;
import java.io.PrintWriter;
import java.util.Scanner;

public class Ula {

    // Retorna o codigo hexadecimal do mnemonico conforme a tabela da ULA
    public static String getOpcode(String m) {
        if (m.equalsIgnoreCase("nA")) return "0";
        if (m.equalsIgnoreCase("AoBn")) return "1";
        if (m.equalsIgnoreCase("nAeB")) return "2";
        if (m.equalsIgnoreCase("zeroL")) return "3";
        if (m.equalsIgnoreCase("AeBn")) return "4";
        if (m.equalsIgnoreCase("nB")) return "5";
        if (m.equalsIgnoreCase("AxB")) return "6";
        if (m.equalsIgnoreCase("AenB")) return "7";
        if (m.equalsIgnoreCase("nAoB")) return "8";
        if (m.equalsIgnoreCase("AxBn")) return "9";
        if (m.equalsIgnoreCase("copiaB")) return "A";
        if (m.equalsIgnoreCase("AeB")) return "B";
        if (m.equalsIgnoreCase("umL")) return "C";
        if (m.equalsIgnoreCase("AonB")) return "D";
        if (m.equalsIgnoreCase("AoB")) return "E";
        if (m.equalsIgnoreCase("copiaA")) return "F";
        return null;
    }

    // Converte o valor de X ou Y (0 a 15) para um digito em hexadecimal
    public static String converteValor(String val, int linha) {
        try {
            int n = Integer.parseInt(val);
            if (n < 0 || n > 15) {
                System.out.println("Erro na linha " + linha + ": valor fora do limite (0-15)");
                return null;
            }
            return Integer.toHexString(n).toUpperCase();
        } catch (Exception e) {
            System.out.println("Erro na linha " + linha + ": valor invalido");
            return null;
        }
    }

    public static void main(String[] args) {
        String x = "0"; // guarda o ultimo valor atribuido a X
        String y = "0"; // guarda o ultimo valor atribuido a Y
        StringBuilder hex = new StringBuilder(); // acumula as instrucoes geradas

        // Leitura e traducao do arquivo de entrada .ula
        try {
            Scanner sc = new Scanner(new File("testeula.ula"));
            int linha = 0;

            while (sc.hasNextLine()) {
                linha++;
                String str = sc.nextLine().trim();

                // Ignora linhas em branco avisando o erro
                if (str.isEmpty()) {
                    System.out.println("Erro na linha " + linha + ": linha em branco");
                    continue;
                }

                // Marcadores de inicio e fim do programa
                if (str.equalsIgnoreCase("inicio:")) continue;
                if (str.equalsIgnoreCase("fim.")) break;

                // Remove espacos para facilitar o parse
                str = str.replace(" ", "");

                // Verifica se o comando termina com ponto e virgula
                if (!str.endsWith(";")) {
                    System.out.println("Erro na linha " + linha + ": faltou ponto e virgula");
                    continue;
                }

                String maiuscula = str.toUpperCase();
                String valor = "";
                if (str.length() > 2) {
                    valor = str.substring(2, str.length() - 1);
                }

                // Trata atribuicao de X
                if (maiuscula.startsWith("X=")) {
                    String vx = converteValor(valor, linha);
                    if (vx != null) x = vx;
                } 
                // Trata atribuicao de Y
                else if (maiuscula.startsWith("Y=")) {
                    String vy = converteValor(valor, linha);
                    if (vy != null) y = vy;
                } 
                // Trata operacao em W e gera a instrucao XYZ
                else if (maiuscula.startsWith("W=")) {
                    String op = getOpcode(valor);
                    if (op != null) {
                        hex.append(x).append(y).append(op).append("\n");
                    } else {
                        System.out.println("Erro na linha " + linha + ": instrucao invalida");
                    }
                } 
                else {
                    System.out.println("Erro na linha " + linha + ": comando desconhecido");
                }
            }
            sc.close();
        } catch (Exception e) {
            System.out.println("Erro ao ler testeula.ula: " + e.getMessage());
            return;
        }

        // Gravacao do arquivo de saida testeula.hex
        try {
            PrintWriter pw = new PrintWriter("testeula.hex");
            pw.print(hex.toString());
            pw.close();
        } catch (Exception e) {
            System.out.println("Erro ao gravar testeula.hex: " + e.getMessage());
            return;
        }

        System.out.println("Arquivo testeula.hex gerado com sucesso:");
        System.out.print(hex.toString());

        // Imprime em linha unica para facilitar colar no Tinkercad
        String tinkercad = hex.toString().trim().replace("\n", " ");
        System.out.println("\nPara o Tinkercad:");
        System.out.println(tinkercad);
    }
}
