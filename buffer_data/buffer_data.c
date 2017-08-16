static void buffer_data(FILE *file, char *buffer, int *buffer_used_size, const int buffer_size,
		const char *send_data, const int len)
{
	assert(file != NULL && buffer != NULL && buffer_used_size != NULL);
	int count = len, written = 0, r = 0;
	while(count > 0)
	{
		if(buffer_size - *buffer_used_size < 2)
		{
			fprintf(file, "%s", buffer);
			*buffer_used_size = 0;
			buffer[0] = '\0';
		}
		r = snprintf (buffer + *buffer_used_size, buffer_size - *buffer_used_size, "%s", send_data + written);
		if(r < 0)
		{										
			return ;
		}
		else if(r >= buffer_size - *buffer_used_size)
		{
			r = buffer_size - *buffer_used_size;
		}
		*buffer_used_size += r;
		written += r;
		count -= r;
	}
	return;
}
