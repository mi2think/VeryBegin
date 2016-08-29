#include <iostream>
#include <string>
#include <sstream>
#include "expr.h"


int main()
{
	Expr t = Expr("*", Expr("-", 5), Expr("+", 3, 4));
	std::cout << t << "=" << t.eval() << std::endl;
	t = Expr("*", t, t);
	std::cout << t << "=" << t.eval() << std::endl;
	t = Expr("/", t, Expr(10));
	std::cout << t << "=" << t.eval() << std::endl;
	t = Expr("+", t, Expr("?:", Expr("/", Expr(5), Expr(10)), Expr(2), Expr(4)));
	std::cout << t << "=" << t.eval() << std::endl;

	system("pause");
	return 0;
}




