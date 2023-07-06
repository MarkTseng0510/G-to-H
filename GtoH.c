///This work is used to transform generator matrix to parity check matrix.
///You must first define the dimension size of the generated matrix in the txt file.
///(first put the number of rows and then the number of columns)
///Then comes the generator matrix itself.
///You can define your own filename, but remember to change the following filename that the program reads.

///The result of the program will return a txt file,
///the first line is the number of rows, the second line is the number of columns,
///and the rest is the parity check matrix.

#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>

#define true 1
#define false 0

//The file includes the generator matrix that you want the program reads.
const char* filename = "G.txt";

int main(void)
{
    ///Read file & transform the contents from "char" to "int"
    ///--------------------------------------------------------------
    FILE *in_file = fopen(filename, "r");
    if (!in_file) {
        perror("fopen");
        exit(EXIT_FAILURE);
    }

    struct stat sb;
    if (stat(filename, &sb) == -1) {
        perror("stat");
        exit(EXIT_FAILURE);
    }

    char *file_contents = malloc(sb.st_size);
    int G_in_int[100];
    int i = 0;

    // Scan all "char" in the file but ignore blank & newline character
    while (fscanf(in_file, "%[^\n ] ", file_contents) != EOF) {
        //printf("> %s", file_contents);
        G_in_int[i] = atoi(file_contents);
        //printf("-- %d", G_in_int[i]);
        i++;
    }

    ///------------------------------------------------------------------------
    //Form the G matrix to store the int input transformed from file
    int n = G_in_int[0];
    int k = G_in_int[1];
    //printf("%d ", n);
    //printf("\n%d \n", k);
    int* G_matrix = malloc(k * n * sizeof(int));
    int z = 2;
    for (int x = 0; x < k; x++) {
        for (int y = 0; y < n; y++) {
            int index = x * n + y;
            G_matrix[index] = G_in_int[z];
            z++;
            //printf("%d ", G_matrix[index]);
        }
        //printf("\n");
    }
    //printf("\n");


    ///Gaussian Elimination
    ///--------------------------------------------------------------

    //1. Make sure the first element of the array G_matrix is 1.
    if (G_matrix[0] == 0) {
        for (int i = 1; i < k; i++) {
            if (G_matrix[i * n] == 1) {
                for (int j = 0; j < n; j++) {
                    G_matrix[j] = G_matrix[j] ^ G_matrix[i * n + j];
                }
            break;
            }
        }
    }



    //2. Find the first 1 that appears in each row,
    //and then eliminate the 1 of the same column and different row
    for (int i = 0; i < k; i++) {
        for(int j = 0; j < n; j++) {

            if (G_matrix[i * n + j] == 1) {

                for (int a = 0; a < k; a++) {

                    if (a == i || G_matrix[a * n + j] == 0)
                        continue;

                    else if (G_matrix[a * n + j] == 1) {
                        for (int jj = 0; jj < n; jj++) {
                            G_matrix[a * n + jj] = G_matrix[a * n + jj] ^ G_matrix[i * n + jj];
                        }
                    }
                }
                break;
            }
        }
    }

    //3. Make sure G is stepped
    int* I_step = malloc(k * sizeof(int));
    int index = 0;
    for (int i = 0; i < k; i++) {
        for (int j = 0; j < n; j++) {
            if (G_matrix[i * n + j] == 1) {
                I_step[index] = j;
                index++;
                break;
            }
        }
    }
    /*
    for (int y = 0; y < k; y++) {
            printf("%d \n", I_step[y]);
    }
    printf("\n");
    */
    int* temp_mat = malloc(n * sizeof(int));
    int temp;
    for (int i = 0; i < (k - 1); i++) {
        for (int j = 0; j < (k - 1 - i); j++) {
            if (I_step[j] > I_step[j+1]) {
                temp = I_step[j];
                I_step[j] = I_step[j+1];
                I_step[j+1] = temp;

                for (int x = 0; x < n; x++) {
                    temp_mat[x] = G_matrix[j * n + x];
                    G_matrix[j * n + x] = G_matrix[(j + 1) * n + x];
                    G_matrix[(j + 1) * n + x] = temp_mat[x];
                }
            }
        }
    }
/*
    for (int x = 0; x < k; x++) {
        for (int y = 0; y < n; y++) {
            printf("%d ", G_matrix[x * n + y]);
        }
        printf("\n");
    }
    printf("\n");
*/

    ///------------------------------------------------------------------


    ///Transform G to H
    //Find the numbers of rows & columns of H,
    //then find the elements in H
    int H_n = n;
    int H_k = n - k;

    int* I_loc = malloc(n * sizeof(int));
    for (int j = 0; j < n; j++) {
            I_loc[j] = 0;
    }
    for (int i = 0; i < k; i++) {
        for (int j = 0; j < n; j++) {
            if (G_matrix[i * n + j] == 1) {
                I_loc[j] = 1;
                break;
            }
        }
    }
    /*
    for (int j = 0; j < H_n; j++) {
            printf("%d ", I_loc[j]);
    }
    printf("\n\n");
    */

    //1. Complete the part of the identity matrix first
    int* I_mat = malloc(H_k * H_k * sizeof(int));
    for (int i = 0; i < H_k; i++) {
        for (int j = 0; j < H_k; j++) {
            if (i == j)
                I_mat[i * H_k + j] = 1;

            else
                I_mat[i * H_k + j] = 0;

            //printf("%d ", I_mat[i * H_k + j]);
        }
        //printf("\n");
    }
    //printf("\n");


    //2. The part of the non-identity matrix is obtained by transposing the corresponding elements of the G matrix.
    int* A_Tran = malloc(H_k * k * sizeof(int));
    int row_count = 0;
    int col_count = 0;
    for (int i = 0; i < k; i++) {

        row_count = 0;
        for (int j = 0; j < n; j++) {

            if (I_loc[j] == 0) {
                A_Tran[row_count * k + col_count] = G_matrix[i * n + j];
                row_count++;
            }
        }
        col_count++;
    }


    //Combine the matrices obtained in step 1 and step 2 to obtain the H matrix to complete the conversion.
    int* H_matrix = malloc(H_k * H_n * sizeof(int));
    for (int x = 0; x < H_k; x++) {
        for (int y = 0; y < H_n; y++) {
            H_matrix[x * H_n + y] = 0;
        }
    }

    int count = 0;
    for (int i = 0; i < H_k; i++) {
        for (int  j= 0; j < H_n; j++) {
            if (I_loc[j] == 1) {
                H_matrix[i * H_n + j] = A_Tran[count];
                count++;
            }
        }
    }

    count = 0;
    for (int i = 0; i < H_k; i++) {
        for (int  j= 0; j < H_n; j++) {
            if (I_loc[j] == 0) {
                H_matrix[i * H_n + j] = I_mat[count];
                count++;
            }
        }
    }


/*
    printf("%d\n", H_n);
    printf("%d\n", H_k);
    for (int x = 0; x < H_k; x++) {
        for (int y = 0; y < H_n; y++) {
            printf("%d ", H_matrix[x * H_n + y]);
        }
        printf("\n");
    }
    printf("\n");
*/


    ///----------------------------------------------------------------------------
    ///Write the converted H matrix into a txt file
    char H_char[100] = {};
    sprintf(&H_char[0], "%d", H_n);
    H_char[1] = '\n';
    sprintf(&H_char[2], "%d", H_k);
    H_char[3] = '\n';

    z = 4;
    for (int x = 0; x < H_k; x++) {
        for (int y = 0; y < H_n; y++) {
            sprintf(&H_char[z], "%d", H_matrix[x * H_n + y]);
            H_char[z+1] = ' ';
            z = z + 2;
        }
        H_char[z] = '\n';
        z++;
    }

/*
    for (int r = 0; r < 100; r++) {
        printf("%c", H_char[r]);
    }
*/

    printf("Complete! Please see the H.txt in the folder.\n");

    FILE *out_txt;
    out_txt = fopen("H.txt","w");
    fprintf(out_txt, H_char);
    fclose(out_txt);

    getchar();

    free(file_contents);
    free(G_matrix);
    free(H_matrix);
    free(I_step);
    free(temp_mat);
    free(I_loc);
    free(I_mat);
    free(A_Tran);

    fclose(in_file);
    exit(EXIT_SUCCESS);
}
