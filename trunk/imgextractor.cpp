/*
  This file is a part of the
  Amagami PS2 Translation Project

  You may feel free to use this code if you want so.
  We provide no guarantee that this code is working,
  useful or anything else. Use it at your own risk.
*/

#include "imgextractor.h"

ImgExtractor::ImgExtractor() : Module("ImgExtractor")
{
}

int ImgExtractor::extract(QString inputPath, QString outputPath, QString structurePath)
{
    FILE *infile;
    int img_size, num_files, i;
    int *name_offset, *data_offset;
    char outpath[1024];

    char *buf;
    int arcsize, fsize;

    infile = fopen(inputPath.toAscii(), "rb");

    if (infile == NULL)
    {
        printf("Error: Opening failed.\n");
        return 1;
    }

    fread(&img_size, 4, 1, infile);
    fread(&num_files, 4, 1, infile);

    //read IMG into memory
    fseek(infile, 0, SEEK_END);
    arcsize = ftell(infile);
    fseek(infile, 0, SEEK_SET);
    buf = (char*)malloc(arcsize);
    fread(buf, arcsize, 1, infile);
    fclose(infile);

    name_offset = (int*)((char*)buf + 8);
    data_offset = (int*)((char*)buf + 4 * num_files + 8);

    for (i = 0; i < num_files; i++)
    {
        memset(outpath, 0, sizeof(outpath));
        sprintf(outpath, "%s\\%s.scf", outputPath.toAscii().data(), buf + name_offset[i]);
        //printf("%s\t%i\n", outpath, i);

        fsize = (i == num_files - 1) ? arcsize-data_offset[i] : data_offset[i + 1] - data_offset[i];

        infile = fopen(outpath, "wb");
        fwrite(buf + data_offset[i], fsize, 1, infile);
        fclose(infile);
    }

    //create name-list for rebuilder --easy way.
    infile = fopen(structurePath.toAscii(), "wb");
    fwrite(buf + name_offset[0], data_offset[0] - name_offset[0], 1, infile);
    fclose(infile);

    free(buf);

    return 0;
}
