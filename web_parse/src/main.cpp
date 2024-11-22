/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlitran <dlitran@student.42barcelona.co    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/11 12:45:54 by dlitran           #+#    #+#             */
/*   Updated: 2024/11/11 12:45:54 by dlitran          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/ConfFile.hpp"

int main(int argc, char **argv)
{

	if (argc != 2)
	{
		std::cout << "Error: Invalid number of arguments" << std::endl;
		return (0);
	}
	ConfFile a;
	try
	{
		a.parse(std::string(argv[1]));
		a.data_structure();
		a.check();
		a.clean();
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << '\n';
	}
	return (0);
}
