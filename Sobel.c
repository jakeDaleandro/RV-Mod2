#include <stdio.h> /* Sobel.c */
#include <math.h>

int pic[256][256];
int outpicx[256][256];
int outpicy[256][256];
int maskx[3][3] = {{-1, 0, 1}, {-2, 0, 2}, {-1, 0, 1}};
int masky[3][3] = {{1, 2, 1}, {0, 0, 0}, {-1, -2, -1}};
double ival[256][256], maxival;
int rows = 256;
int cols = 256;

int main(int argc, char *argv[])
{
        int i, j, p, q, mr, sum1, sum2;
        double threshold;
        FILE *fo1, *fo2, *fo3, *fp1;
        char *foobar;

        argc--;
        argv++;
        foobar = *argv;
        fp1 = fopen(foobar, "rb");

        argc--;
        argv++;
        foobar = *argv;
        fo1 = fopen(foobar, "wb");
        fprintf(fo1, "P5\n");
        fprintf(fo1, "%d %d\n", cols, rows);
        fprintf(fo1, "255\n");

        fo2 = fopen("sobelout1.pgm", "wb");
        fo3 = fopen("sobelout2.pgm", "wb");

        /* headers */
        fprintf(fo2, "P5\n%d %d\n255\n", cols, rows);
        fprintf(fo3, "P5\n%d %d\n255\n", cols, rows);

        argc--;
        argv++;
        foobar = *argv;
        threshold = atof(foobar);

        double high_thresh = threshold * 1.1;
        double low_thresh = threshold * 0.375;

        for (i = 0; i < 256; i++)
        {
                for (j = 0; j < 256; j++)
                {
                        pic[i][j] = getc(fp1);
                        pic[i][j] &= 0377;
                }
        }

        mr = 1;
        for (i = mr; i < 256 - mr; i++)
        {
                for (j = mr; j < 256 - mr; j++)
                {
                        sum1 = 0;
                        sum2 = 0;
                        for (p = -mr; p <= mr; p++)
                        {
                                for (q = -mr; q <= mr; q++)
                                {
                                        sum1 += pic[i + p][j + q] * maskx[p + mr][q + mr];
                                        sum2 += pic[i + p][j + q] * masky[p + mr][q + mr];
                                }
                        }
                        outpicx[i][j] = sum1;
                        outpicy[i][j] = sum2;
                }
        }

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

                        /* sobel magnitude image */
                        fprintf(fo1, "%c", (unsigned char)(ival[i][j]));

                        /* high threshold image */
                        if (ival[i][j] >= high_thresh)
                                fprintf(fo3, "%c", 255);
                        else
                                fprintf(fo3, "%c", 0);

                        /* low threshold image */
                        if (ival[i][j] >= low_thresh)
                                fprintf(fo2, "%c", 255);
                        else
                                fprintf(fo2, "%c", 0);
                }
        }
}