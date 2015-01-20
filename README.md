# libshare

### test

 **StackEdit**

```c
int test(){
	int ret =0;
	ret = std_write_file("/opt/colin/zhang/hi","hello\n",6);
	printf("ret = %d\n",ret);
	ret = std_write_file("/opt/zhang","hello\n",6);
	printf("ret = %d\n", ret);
	ret = std_write_file("hi","hello\n",6);
	printf("ret = %d\n",ret );	
	return 0;
}
```

> **Note:**

> - Should be carefull with some function.
> - Can alse find [libshare] on the web.


[libshare]: https://github.com/colin-zhang/libshare