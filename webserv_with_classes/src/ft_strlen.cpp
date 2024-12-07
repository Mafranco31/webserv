#include "../inc/header.hpp"

std::string	ft_strlen(std::string s)
{
	size_t	i;

	i = 0;
	while (s[i])
		i++;

    std::stringstream ss;

    // Convierte size_t a cadena de caracteres
    ss << i;
    
    // Obtiene el valor como std::string
    std::string len_body = ss.str();
	return (len_body);
}