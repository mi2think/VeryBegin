#include "expr.h"

Expr::Expr(int n) : p(new IntNode(n))
{
}

Expr::Expr(const std::string& op, Expr e) : p(new UnaryNode(op, e))
{
}

Expr::Expr(const std::string& op, Expr left, Expr right) : p(new BinaryNode(op, left, right))
{
}

Expr::Expr(const std::string& op, Expr left, Expr middle, Expr right) : p (new TernaryNode(op, left, middle, right))
{
}

Expr::Expr(const Expr& e) : p(e.p)
{
	++p->use;
}

Expr::~Expr()
{
	if (--p->use == 0)
		delete p;
}

Expr& Expr::operator = (const Expr& e)
{
	if (this != &e)
	{
		if (--p->use == 0)
			delete p;
		p = e.p;
		++p->use;
	}
	return *this;
}

int Expr::eval() const 
{
	return p->eval();
}

std::ostream& operator << (std::ostream& os, const Expr& e)
{
	e.p->print(os);
	return os;
}
