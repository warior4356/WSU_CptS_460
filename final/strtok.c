char* strtok(char * line, char * tokens, )
{
    char* tmp = line;
    int token = 0;
      
    if(*line == '\0')
    {
        return 0;
    }

    while(!token)
    {
        for(int i = 0; i < (strlen(tokens)); i++)
        {
            if(*line == tokens[i] || *line == '\0')
            {
                token = 1;
            }
        }
        if(!token)
        {
            line++;
        }
    }
    *line = '\0';
    line++;
    return tmp;
}
