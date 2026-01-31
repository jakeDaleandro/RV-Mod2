#include <stdio.h>
#include <math.h>
#include <stdlib.h>

#define PICSIZE 256
#define MAXMASK 100

int pic[PICSIZE][PICSIZE];
double outpicx[PICSIZE][PICSIZE];
double outpicy[PICSIZE][PICSIZE];
double ival[PICSIZE][PICSIZE];

double xmask[MAXMASK][MAXMASK];
double ymask[MAXMASK][MAXMASK];

int main(int argc, char *argv[])
{
    int i, j, p, q, mr, centx, centy;
    double sig, maxival = 0.0;
    FILE *fp1, *fo1;
    char *foobar;

    /* ---- Argument handling ---- */
    argc--; argv++; foobar = *argv; fp1 = fopen(foobar, "rb");
    argc--; argv++; foobar = *argv; fo1 = fopen(foobar, "wb");
    argc--; argv++; foobar = *argv; sig = atof(foobar);

    fprintf(fo1, "P5\n%d %d\n255\n", PICSIZE, PICSIZE);

    /* ---- Read image ---- */
    for (i = 0; i < PICSIZE; i++)
        for (j = 0; j < PICSIZE; j++)
            pic[i][j] = getc(fp1) & 0377;

    /* ---- Build Gaussian derivative masks ---- */
    mr = (int)(sig * 3);
    centx = MAXMASK / 2;
    centy = MAXMASK / 2;

    for (p = -mr; p <= mr; p++)
    {
        for (q = -mr; q <= mr; q++)
        {
            double exponent = exp(-((p*p + q*q) / (2 * sig * sig)));

            xmask[p + centy][q + centx] = (-p / (sig * sig)) * exponent;
            ymask[p + centy][q + centx] = (-q / (sig * sig)) * exponent;
        }
    }

    /* ---- Convolution (compute gradients) ---- */
    for (i = mr; i < PICSIZE - mr; i++)
    {
        for (j = mr; j < PICSIZE - mr; j++)
        {
            double sumx = 0.0;
            double sumy = 0.0;

            for (p = -mr; p <= mr; p++)
            {
                for (q = -mr; q <= mr; q++)
                {
                    sumx += pic[i+p][j+q] * xmask[p+centy][q+centx];
                    sumy += pic[i+p][j+q] * ymask[p+centy][q+centx];
                }
            }

            outpicx[i][j] = sumx;
            outpicy[i][j] = sumy;
        }
    }

    /* ---- Gradient magnitude ---- */
    for (i = mr; i < PICSIZE - mr; i++)
    {
        for (j = mr; j < PICSIZE - mr; j++)
        {
            ival[i][j] = sqrt(
                outpicx[i][j] * outpicx[i][j] +
                outpicy[i][j] * outpicy[i][j]);

            if (ival[i][j] > maxival)
                maxival = ival[i][j];
        }
    }

    /* ---- Normalize + output ---- */
    for (i = 0; i < PICSIZE; i++)
    {
        for (j = 0; j < PICSIZE; j++)
        {
            ival[i][j] = (ival[i][j] / maxival) * 255.0;
            fprintf(fo1, "%c", (unsigned char)(ival[i][j]));
        }
    }

    fclose(fp1);
    fclose(fo1);
    return 0;
}
