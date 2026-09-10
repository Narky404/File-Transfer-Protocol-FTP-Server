import java.io.BufferedReader;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.io.InputStreamReader;

public class RecherchePLSSC {

    // Recherche d'une PLSSC avec un algorithme naïf
    public static String PLSSC(String S1, String S2) {

        int i = S1.length();
        int j = S2.length();

        // Si l'une des deux chaînes est vide, il n'y a pas de sous-séquence commune
        if (i == 0 || j == 0) {
            return "";
        }

        // Si les derniers caractères des deux chaînes sont identiques
        if (S1.charAt(i - 1) == S2.charAt(j - 1)) {

            // On ajoute ce caractère à la solution et on continue avec les chaînes
            // raccourcies
            return PLSSC(S1.substring(0, i - 1), S2.substring(0, j - 1)) + S1.charAt(i - 1);

        } else {
            // Sinon, on explore les deux possibilités :
            // en retirant le dernier caractère de S1
            // en retirant le dernier caractère de S2
            String option1 = PLSSC(S1.substring(0, i - 1), S2);
            String option2 = PLSSC(S1, S2.substring(0, j - 1));

            // On retourne la plus longue sous-séquence trouvée parmi les deux options
            if (option1.length() > option2.length()) {
                return option1;
            } else {
                return option2;
            }
        }
    }

    // Recherche d'une PLSSC de 2 chaînes, prog. dyn.
    public static String PLSSC_PD(String S1, String S2) {
        int m = S1.length();
        int n = S2.length();

        // Création d'un tableau dp de taille (m+1) x (n+1) pour stocker les résultats
        // intermédiaires
        int[][] dp = new int[m + 1][n + 1];

        for (int i = 1; i <= m; i++) {
            for (int j = 1; j <= n; j++) {

                // Si les caractères actuels de S1 et S2 sont identiques
                if (S1.charAt(i - 1) == S2.charAt(j - 1)) {

                    // On ajoute 1 à la valeur de la diagonale (dp[i-1][j-1])
                    dp[i][j] = dp[i - 1][j - 1] + 1;
                } else {

                    // Sinon, on prend la valeur maximale entre la case du haut (dp[i-1][j])
                    // et la case de gauche (dp[i][j-1])
                    dp[i][j] = Math.max(dp[i - 1][j], dp[i][j - 1]);
                }
            }
        }

        // Reconstruction de la PLSSC à partir du tableau dp
        StringBuilder plssc = new StringBuilder();
        int i = m, j = n;

        while (i > 0 && j > 0) {

            // Si les caractères actuels de S1 et S2 sont identique
            if (S1.charAt(i - 1) == S2.charAt(j - 1)) {

                // On ajoute ce caractère à la PLSSC
                plssc.append(S1.charAt(i - 1));
                i--;
                j--;
            } else if (dp[i - 1][j] > dp[i][j - 1]) {

                // Si la valeur du haut est plus grande, on se déplace vers le haut
                i--;

            } else {

                // Sinon, on se déplace vers la gauche
                j--;
            }
        }

        // La PLSSC est construite à l'envers, donc on la retourne avant de la retourner
        return plssc.reverse().toString();
    }

    public static void main(String args[]) {

        String S1;
        String S2;

        FileInputStream input;
        BufferedReader reader;

        for (int i = 0; i < args.length; i++) {
            try {
                // Ouverture du fichier passé en argument
                input = new FileInputStream(args[i]);
                reader = new BufferedReader(new InputStreamReader(input));

                // Lecture de S1
                S1 = reader.readLine();
                // Lecture S2
                S2 = reader.readLine();

                // date de début
                long startTime = System.nanoTime();

                // String result = PLSSC(S1, S2);
                String result = PLSSC_PD(S1, S2);

                // date de fin pour le calcul du temps écoulé
                long endTime = System.nanoTime();

                System.out.println("PLSSC: " + result);

                // Impression de la longueur du S1 de S2 et du temps d'exécution
                System.out.println(S1.length() + "\t" + S2.length() + "\t" + ((endTime - startTime) / 1.0E9));

            } catch (FileNotFoundException e) {
                System.err.println("Erreur lors de l'ouverture du fichier " + args[i]);
            } catch (IOException e) {
                System.err.println("Erreur de lecture dans le fichier");
            }
        }
    }
}