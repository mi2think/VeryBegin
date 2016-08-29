#pragma once

#include <iostream>
#include <string>
#include <sstream>

class ExprNode;
class Expr
{
public:
	Expr(int n);
	Expr(const std::string& op, Expr e);
	Expr(const std::string& op, Expr left, Expr right);
	Expr(const std::string& op, Expr left, Expr middle, Expr right);
	Expr(const Expr& e);
	Expr& operator=(const Expr& e);
	~Expr();
	int eval() const;
private:
	friend std::ostream& operator << (std::ostream& os, const Expr& e);
	ExprNode* p;
};

class ExprNode
{
public:
	ExprNode() : use(1) {}
	virtual ~ExprNode() {}
	virtual void print(std::ostream&) const = 0;
	virtual int eval() const = 0;
private:
	friend std::ostream& operator<<(std::ostream&, const ExprNode&);
	friend class Expr;
	int use;
};

class IntNode : public ExprNode
{
public:
	IntNode(int _n) : n(_n) {}
	void print(std::ostream& os) const { os << n; }
	int eval() const { return n; }
private:
	int n;
};

class UnaryNode : public ExprNode
{
public:
	UnaryNode(const std::string& _op, Expr _e)
		: op(_op), e(_e) {}
	void print(std::ostream& os) const 
	{
		os << '(' << op << e << ')';
	}
	int eval() const { if (op == "-") return -e.eval(); throw "bad op" + op + "in UnaryNode"; }
private:
	std::string op;
	Expr e;
};

class BinaryNode : public ExprNode
{
public:
	BinaryNode(const std::string& _op, Expr _left, Expr _right)
		: op(_op), left(_left), right(_right) {}
	void print(std::ostream& os) const
	{
		os << '(' << left << op << right << ')';
	}
	int eval() const 
	{
		int op1 = left.eval();
		int op2 = right.eval();
		if (op == "-") return op1 - op2;
		if (op == "+") return op1 + op2;
		if (op == "*") return op1 * op2;
		if (op == "/" && op2 != 0) return op1 / op2;
		throw "error bad op" + op + "in BinaryNode";
	}
private:
	std::string op;
	Expr left;
	Expr right;
};

class TernaryNode : public ExprNode
{
public:
	TernaryNode(const std::string& _op, Expr a, Expr b, Expr c)
		: op(_op), left(a), middle(b), right(c) {}
	void print(std::ostream& os) const
	{
		os << "(" << left << " ? " << middle << " : " << right << ")";
	}
	int eval() const
	{
		if (left.eval())
			return middle.eval();
		else
			return right.eval();
	}
private:
	std::string op;
	Expr left;
	Expr middle;
	Expr right;
};

