#include <stdio.h> /* Standard I/O (file reading/writing) */
#include <math.h>  /* exp(), fabs() */

#define PICSIZE 256 /* Image assumed to be 256x256 grayscale */
#define MAXMASK 100 /* Max size of convolution mask */

/* -------------------- Global Images & Buffers -------------------- */

/* Original input image (0–255 grayscale) */
int pic[PICSIZE][PICSIZE];

/* Output after LoG convolution (before zero-crossing step) */
double outpicx[PICSIZE][PICSIZE];
double outpicy[PICSIZE][PICSIZE];

/* Final edge image (binary) */
double ival[PICSIZE][PICSIZE];

/* Laplacian of Gaussian (LoG) mask */
double xmask[MAXMASK][MAXMASK];
double ymask[MAXMASK][MAXMASK];

/* Stores convolution result for zero-crossing detection */
double convx[PICSIZE][PICSIZE];
double convy[PICSIZE][PICSIZE];

/* --------------------------- MAIN --------------------------- */
int main(int argc, char *argv[])
{
    int i, j, p, q, x, y, mr, centx, centy;
    double xmaskval, ymaskval, sum, sig, maxival, minival, maxval;

    FILE *fp1, *fo1;
    char *foobar;

    /* ---- Argument handling ---- */
    argc--;
    argv++;
    foobar = *argv;
    fp1 = fopen(foobar, "rb");

    argc--;
    argv++;
    foobar = *argv;
    fo1 = fopen(foobar, "wb");
    fprintf(fo1, "P5\n");
    fprintf(fo1, "%d %d\n", PICSIZE, PICSIZE);
    fprintf(fo1, "255\n");

    argc--;
    argv++;
    foobar = *argv;
    sig = atof(foobar);

    /* Mask radius = 3*sigma (common rule of thumb) */
    mr = (int)(sig * 3);

    /* Center of mask array */
    centx = (MAXMASK / 2);
    centy = (MAXMASK / 2);

    /* ---------------- Read Input Image ---------------- */
    for (i = 0; i < 256; i++)
    {
        for (j = 0; j < 256; j++)
        {
            pic[i][j] = getc(fp1);
        }
    }

    /* ---------------- Build LoG Mask ----------------
       LoG(x,y) = (2 - (x^2+y^2)/σ^2) * exp(-(x^2+y^2)/(2σ^2))
    -------------------------------------------------- */
    for (x = -mr; x <= mr; x++)
    {
        for (y = -mr; y <= mr; y++)
        {
            xmaskval = x * exp(-1 * (((x * x) + (y * y)) / (2 * (sig * sig))));
            ymaskval = y * exp(-1 * (((x * x) + (y * y)) / (2 * (sig * sig))));
            xmask[x + centy][y + centx] = xmaskval;
            ymask[x + centy][y + centx] = ymaskval;
        }
    }
    /* ---------------- Convolution ----------------
       Apply LoG filter to image
    ----------------------------------------------- */
    for (i = mr; i <= 255 - mr; i++)
    {
        for (j = mr; j <= 255 - mr; j++)
        {
            sum = 0;
            for (p = -mr; p <= mr; p++)
            {
                for (q = -mr; q <= mr; q++)
                {
                    sum += pic[i + p][j + q] * xmask[p + centy][q + centx];
                }
            }
            outpicx[i][j] = sum; /* For display */
            convx[i][j] = sum;   /* For zero-cross detection */
        }
    }

    for (i = mr; i <= 255 - mr; i++)
    {
        for (j = mr; j <= 255 - mr; j++)
        {
            sum = 0;
            for (p = -mr; p <= mr; p++)
            {
                for (q = -mr; q <= mr; q++)
                {
                    sum += pic[i + p][j + q] * ymask[p + centy][q + centx];
                }
            }
            outpicy[i][j] = sum; /* For display */
            convy[i][j] = sum;   /* For zero-cross detection */
        }
    }

    /* ---------------- Normalize Output ----------------
       Scale convolution result to 0–255 for viewing
    --------------------------------------------------- */
    maxval = 0;
    maxival = 0;
    minival = 255;
    for (i = mr; i < 256 - mr; i++)
    {
        for (j = mr; j < 256 - mr; j++)
        {
            ival[i][j] = sqrt((double)((outpicx[i][j] * outpicx[i][j]) +
                                       (outpicy[i][j] * outpicy[i][j])));
            if (ival[i][j] > maxival)
                maxival = ival[i][j];
        }
    }

    for (i = 0; i < 256; i++)
    {
        for (j = 0; j < 256; j++)
        {
            /* normalize magnitude */
            ival[i][j] = (ival[i][j] / maxival) * 255.0;

            /* sobel magnitude image */
            fprintf(fo1, "%c", (unsigned char)(ival[i][j]));
        }
    }
}