inline void mulmatrix4x4(float ma[4][4], float mb[4][4], float mout[4][4])
{
	char x=0;
	char y=0;

	while (y!=4)
	{
		//gcc will generate fuse-multiply-add instructions with -O2 and above, remember to -march=native !
		mout[y][x] = ma[y][0]*mb[0][x] + ma[y][1]*mb[1][x] + ma[y][2]*mb[2][x] + ma[y][3]*mb[3][x];
		x++;
		if (x==4)
		{
			x=0;
			y++;
		}
	}
}
