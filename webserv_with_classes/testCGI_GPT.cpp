#include <iostream>
#include <string>
#include <sstream>
#include <cstdlib>
#include <cstring>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <vector>
#include <map>

#define BUFFER_SIZE 4096


static	int	getlenght(int n)
{
	int	count;

	if (n == -2147483648)
		return (10);
	if (n < 0)
		n *= -1;
	count = 1;
	while (n > 9)
	{
		n /= 10;
		count++;
	}
	return (count);
}

static	char	*makemaxint(char *nbr)
{
	nbr[1] = '2';
	nbr[2] = '1';
	nbr[3] = '4';
	nbr[4] = '7';
	nbr[5] = '4';
	nbr[6] = '8';
	nbr[7] = '3';
	nbr[8] = '6';
	nbr[9] = '4';
	nbr[10] = '8';
	return (nbr);
}

static	char	*getnum(char *ret, int n, int lenght, int negative)
{
	if (negative == 1)
	{
		ret[0] = 45;
		n *= -1;
	}
	ret[lenght + negative] = '\0';
	if (n == -2147483648)
		ret = makemaxint(ret);
	else
	{
		while (lenght > 0)
		{
			ret[lenght + negative - 1] = (n % 10) + 48;
			n /= 10;
			lenght--;
		}
	}
	return (ret);
}

char	*ft_itoa(int n)
{
	char	*ret;
	int		negative;
	int		lenght;

	negative = 0;
	if (n < 0)
		negative = 1;
	lenght = getlenght(n);
	ret = (char *)malloc(lenght + negative + 1);
	if (!ret)
		return (NULL);
	return (getnum(ret, n, lenght, negative));
}

// Function to parse HTTP headers
std::map<std::string, std::string> parseHeaders(const std::string& headers) {
    std::map<std::string, std::string> headerMap;
    std::istringstream stream(headers);
    std::string line;

    while (std::getline(stream, line) && line != "\r") {
        size_t delimiter = line.find(": ");
        if (delimiter != std::string::npos) {
            std::string key = line.substr(0, delimiter);
            std::string value = line.substr(delimiter + 2);
            if (!value.empty() && value.back() == '\r') {
                value.pop_back();
            }
            headerMap[key] = value;
        }
    }

    return headerMap;
}

// Function to execute PHP-CGI and send POST data
std::string executePhpCgi(const std::string& scriptPath, const std::string& postData, const std::map<std::string, std::string>& envVars) {
    int inPipe[2], outPipe[2];
    if (pipe(inPipe) == -1 || pipe(outPipe) == -1) {
        perror("pipe");
        exit(1);
    }

    pid_t pid = fork();
    if (pid < 0) {
        perror("fork");
        exit(1);
    } else if (pid == 0) { // Child process
        // Redirect stdin and stdout
        dup2(inPipe[0], STDIN_FILENO);
        dup2(outPipe[1], STDOUT_FILENO);

        // Close unused pipe ends
        close(inPipe[1]);
        close(outPipe[0]);

        // Prepare environment variables
        std::vector<std::string> envVec;
        for (std::map<std::string, std::string>::const_iterator it = envVars.begin(); it != envVars.end(); ++it) {
            envVec.push_back(it->first + "=" + it->second);
            std::cout << it->first << " = " << it->second << std::endl;
        }
        std::vector<char*> envp;
        for (size_t i = 0; i < envVec.size(); ++i) {
            envp.push_back(const_cast<char*>(envVec[i].c_str()));
        }
        envp.push_back(NULL);

        // Prepare arguments for execve
        const char* args[] = {"php-cgi", scriptPath.c_str(), NULL};

        // Execute PHP-CGI
        execve("/usr/local/bin/php-cgi", const_cast<char* const*>(args), envp.data());
        perror("execve");
        exit(1);
    } else { // Parent process
        // Close unused pipe ends
        close(inPipe[0]);
        close(outPipe[1]);

        // Write POST data to PHP-CGI
        write(inPipe[1], postData.c_str(), postData.size());
        close(inPipe[1]);

        // Read response from PHP-CGI
        char buffer[BUFFER_SIZE];
        std::string response;
        ssize_t bytesRead;
        while ((bytesRead = read(outPipe[0], buffer, BUFFER_SIZE)) > 0) {
            response.append(buffer, bytesRead);
        }
        close(outPipe[0]);

        // Wait for the child process to finish
        waitpid(pid, NULL, 0);

        return response;
    }

    return "";
}

int main(int argc, char **argv) {
    // Simulate receiving HTTP headers and POST data
    std::string postData = "------WebKitFormBoundarypwU7QDV3czBcGsB6\nContent-Disposition: form-data; name=\"file\"; filename=\"test.txt\"\nContent-Type: text/plain\n\nyooo\nyooo\n\n------WebKitFormBoundarypwU7QDV3czBcGsB6--\n";
    (void)argc;
    (void)argv;
    // std::string postData = std::string(argv[1]);
    std::string headers = "Content-Type: multipart/form-data;boundary=----WebKitFormBoundarypwU7QDV3czBcGsB6\r\nContent-Length: "+std::string(ft_itoa(postData.size()))+"\r\n\r\n";
    
    std::cout << "PostData = " << postData << "$" << std::endl;
    // std::cout << "PostData2= " << postData2 << "$" << std::endl;
    // Parse headers
    std::map<std::string, std::string> headerMap = parseHeaders(headers);

    // Set up environment variables for PHP-CGI
    std::map<std::string, std::string> envVars;
    envVars["REQUEST_METHOD"] = "POST";
    envVars["CONTENT_TYPE"] = headerMap["Content-Type"];
    envVars["CONTENT_LENGTH"] = headerMap["Content-Length"];
    envVars["REDIRECT_STATUS"] = "1";
    envVars["SCRIPT_FILENAME"] = "./script.php"; // Change this to your PHP script path

    // Execute PHP-CGI and get the response
    std::string response = executePhpCgi(envVars["SCRIPT_FILENAME"], postData, envVars);

    // Output the response
    // std::cout << "HTTP/1.1 200 OK\r\n";
    // std::cout << "Content-Type: text/html\r\n\r\n";
    std::cout << response;

    return 0;
}
