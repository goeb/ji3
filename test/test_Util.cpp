
#include "Util.hpp"

void printToken(const std::string & line)
{
    std::cout << "printToken: line=" << line << "\n";

    std::vector<std::string> tokens;
    bool r = Util::splitQuotes(line, tokens);
    std::cout << "r=" << r;

    std::vector<std::string>::iterator it;
    std::cout << ", tokens= [ ";
    for (it = tokens.begin(); it != tokens.end(); it++) {
        if (it != tokens.begin()) std::cout << ", ";
        std::cout << *it;
    }
    std::cout << " ]\n";
}

main()
{
    printToken("a b c def");
    printToken("\"a b\" c \"def\" auau");
    printToken("a b\" c \"def\" auau");
    printToken("a b \"ci d ef\" auaui \"xx yy\" \"x\"");

}
