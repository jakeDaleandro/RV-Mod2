#include <stdio.h> /* Standard I/O (file reading/writing) */
#include <math.h>  /* exp(), fabs() */
#include <stdlib.h>
#include <string.h>

#define PICSIZE 256 /* Image assumed to be 256x256 grayscale */
#define MAXMASK 100 /* Max size of convolution mask */

/* -------------------- Global Images & Buffers -------------------- */

/* Original input image (0–255 grayscale) */
int pic[PICSIZE][PICSIZE];

/* Output after LoG convolution (before zero-crossing step) */
double outpicx[PICSIZE][PICSIZE];
double outpicy[PICSIZE][PICSIZE];

/* Final magnitude image (binary) */
double ival[PICSIZE][PICSIZE];

/* Final peaks image (binary) */
double cand[PICSIZE][PICSIZE];

/* Final output image (binary) */
double final[PICSIZE][PICSIZE];

/* Laplacian of Gaussian (LoG) mask */
double xmask[MAXMASK][MAXMASK];
double ymask[MAXMASK][MAXMASK];

/* Stores convolution result for zero-crossing detection */
double xconv[PICSIZE][PICSIZE];
double yconv[PICSIZE][PICSIZE];

int histogram[PICSIZE];

/* --------------------------- MAIN --------------------------- */
int main(int argc, char *argv[])
{
    int i, j, p, q, x, y, mr, centx, centy, HI, LO, percent;
    double xmaskval, ymaskval, sum, sig, maxival, slope;
    HI = 255;

    FILE *fp1, *fo1, *fo2, *fo3;
    char *foobar, *foobar1;

    /* ---- Argument handling ---- */
    // Inputs
    argc--;
    argv++;
    foobar = *argv;
    fp1 = fopen(foobar, "rb");
    if (!fp1)
    {
        perror("fopen");
        exit(1);
    }

    char header[256];
    int width, height, maxval;
    /* read magic number */
    fgets(header, sizeof(header), fp1);
    if (strncmp(header, "P5", 2) != 0)
    {
        printf("Unsupported format (expected P5)\n");
        exit(1);
    }

    /* read width and height */
    do
    {
        fgets(header, sizeof(header), fp1);
    } while (header[0] == '#');

    sscanf(header, "%d %d", &width, &height);

    /* read maxval */
    do
    {
        fgets(header, sizeof(header), fp1);
    } while (header[0] == '#');

    sscanf(header, "%d", &maxval);
    // Outputs
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
    fo2 = fopen(foobar, "wb");
    fprintf(fo2, "P5\n");
    fprintf(fo2, "%d %d\n", PICSIZE, PICSIZE);
    fprintf(fo2, "255\n");

    argc--;
    argv++;
    foobar = *argv;
    fo3 = fopen(foobar, "wb");
    fprintf(fo3, "P5\n");
    fprintf(fo3, "%d %d\n", PICSIZE, PICSIZE);
    fprintf(fo3, "255\n");

    // Number inputs
    argc--;
    argv++;
    foobar = *argv;
    sig = atof(foobar);

    argc--;
    argv++;
    foobar1 = *argv;
    percent = atof(foobar1);

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
            xconv[i][j] = sum;   /* For zero-cross detection */
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
            yconv[i][j] = sum;   /* For zero-cross detection */
        }
    }

    /* ---------------- Getting Peaks ----------------
       use magnitude to find peaks and save in new output
    --------------------------------------------------- */
    maxival = 0;
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
            /* canny magnitude image */
            fprintf(fo3, "%c", (unsigned char)(ival[i][j]));
        }
    }

    for (i = mr; i < 256 - mr; i++)
    {
        for (j = mr; j < 256 - mr; j++)
        {

            if ((xconv[i][j]) == 0.0)
            {
                xconv[i][j] = .00001;
            }
            slope = yconv[i][j] / xconv[i][j];
            if ((slope <= .4142) && (slope > -.4142))
            {
                if ((ival[i][j] > ival[i + 1][j]) && (ival[i][j] > ival[i - 1][j]))
                {
                    cand[i][j] = 255;
                }
            }
            else if ((slope <= 2.4142) && (slope > .4142))
            {
                if ((ival[i][j] > ival[i - 1][j - 1]) && (ival[i][j] > ival[i + 1][j + 1]))
                {
                    cand[i][j] = 255;
                }
            }
            else if ((slope <= -.4142) && (slope > -2.4142))
            {
                if ((ival[i][j] > ival[i + 1][j - 1]) && (ival[i][j] > ival[i - 1][j + 1]))
                {
                    cand[i][j] = 255;
                }
            }
            else
            {
                if ((ival[i][j] > ival[i][j + 1]) && (ival[i][j] > ival[i][j - 1]))
                {
                    cand[i][j] = 255;
                }
            }
        }
    }
    for (i = 0; i < 256; i++)
    {
        for (j = 0; j < 256; j++)
        {
            /* peaks image */
            fprintf(fo2, "%c", (unsigned char)(cand[i][j]));
        }
    }

    /* clear histogram */
    for (i = 0; i < PICSIZE; i++)
    {
        histogram[i] = 0;
    }

    /* build histogram */
    for (i = 0; i < PICSIZE; i++)
    {
        for (j = 0; j < PICSIZE; j++)
        {
            if (ival[i][j] != 0)
                histogram[i] += 1; // cand is 0 or 255
        }
    }

    int areaOfTops = 0;
    int cutoff = (percent * PICSIZE * PICSIZE * 0.01);

    /* find HI */
    for (i = PICSIZE - 1; i >= 0; i--)
    {
        areaOfTops += histogram[i];

        if (areaOfTops >= cutoff)
        {
            HI = i;
            break;
        }
    }

    LO = (int)(0.35 * HI);

    /* --- FIRST PASS: strong / weak rejection --- */
    for (i = 0; i < PICSIZE; i++)
    {
        for (j = 0; j < PICSIZE; j++)
        {
            if (cand[i][j] != 0) // peaks == ON
            {
                if (ival[i][j] > HI)
                {
                    cand[i][j] = 0;    // peaks OFF
                    final[i][j] = 255; // final ON
                }
                else if (ival[i][j] < LO)
                {
                    cand[i][j] = 0;  // peaks OFF
                    final[i][j] = 0; // final OFF
                }
                /* else: weak peak, keep in cand for hysteresis */
            }
        }
    }

    /* --- HYSTERESIS LOOP --- */
    int moretodo = 1;

    while (moretodo)
    {
        moretodo = 0;

        for (i = 0; i < PICSIZE; i++)
        {
            for (j = 0; j < PICSIZE; j++)
            {
                if (cand[i][j] != 0) // still a weak peak
                {
                    for (p = -1; p <= 1; p++)
                    {
                        for (q = -1; q <= 1; q++)
                        {
                            int ni = i + p;
                            int nj = j + q;

                            /* bounds check */
                            // if (ni < 0 || ni >= PICSIZE || nj < 0 || nj >= PICSIZE)
                            //   continue;

                            if (final[ni][nj] != 0)
                            {
                                cand[i][j] = 0;    // peaks OFF
                                final[i][j] = 255; // final ON
                                moretodo = 1;
                            }
                        }
                    }
                }
            }
        }
    }

    for (i = 0; i < 256; i++)
    {
        for (j = 0; j < 256; j++)
        {
            /* sobel magnitude image */
            fprintf(fo1, "%c", (unsigned char)(final[i][j]));
        }
    }
}