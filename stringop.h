bool strcmpl(char* a, char* b)
{
	int q = strlen(a);
	int p = strlen(b);
	if(q < p)
		return false;

	for(int i = 0; i < p; i++)
	{
		if(a[i] != b[i])
			return false;
	}

	return true;
}