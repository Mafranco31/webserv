#include "../inc/header.hpp"

int	getlenght(int n)
{
	int	count;

	if (n == -2147483648)
		return (10);
	if (n < 0)
		n *= -1;
	count = 1;
	while (n > 9) {
		n /= 10;
		count++;
	}
	return (count);
}

std::string	getnum(int n, int lenght, int negative)
{
	std::string ret = "";

	if (n == -2147483648)
		return "-2147483648";
	if (negative == 1) {
		ret += 45;
		n *= -1;
	}
	while (lenght > 0) {
		ret += (n % 10) + 48;
		n /= 10;
		lenght--;
	}
	return (ret);
}

std::string	ft_itoa(int n)
{
	std::string	ret;
	int		negative;
	int		lenght;

	negative = 0;
	if (n < 0)
		negative = 1;
	lenght = getlenght(n);
	return (getnum(n, lenght, negative));
}