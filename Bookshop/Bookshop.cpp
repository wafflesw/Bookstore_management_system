//this program uses mysql
#include <iostream>
#include <windows.h>
#include <mysql.h>
#include <sstream>
#include <conio.h>
#include <iomanip>  
#define HOST "localhost"// default host
#define USER "root"// just default user
#define PASS /*Enter password here*/ //password to get into mysql
#define DATABASE "BK_MANG" //name of database but can change this to match any database
#define PORT 3306 // default port
#define PASSWORD 1234 //password to be able to log into the menu


MYSQL* conn;
MYSQL_RES* res_set;
MYSQL_ROW row;
std::stringstream stmt;
const char* Q;
std::string query;

typedef struct
{
	int date;
	int month;
	int year;
}date;

class book
{
	int bk_id;
	std::string bk_name;
	std::string bk_auth;
	std::string price;//I use a string here instead of a double to avoid persion issues that I was having with whole numbers and same goes for other prices 
	int qty;
public:
	void add();
	void update_price();
	void search();
	void update();
	void display();
};

class supplier
{
	int sup_id;
	std::string sup_name;
	std::string sup_email;
	long long int sup_num;
	std::string sup_address1;
	std::string sup_address2;
	std::string sup_city;
	std::string sup_state;
	
public:

	void add_sup();
	void rem_sup();
	void search_sup_id();

};

class purchase
{
	int ord_id;
	int book_id;
	int supplier_id;
	int qty;
	date dt_ordered;
	int eta;
	char received;
	int inv;

public:
	void new_order();
	void view();
	void mark_cancel();
	void mark_recive();
};

class employee
{
	int emp_id;
	std::string emp_name;
	std::string emp_email;
	long long int emp_num;
	std::string emp_addres1;
	std::string emp_addres2;
	std::string city;
	std::string state;
	date hire_date;
	std::string sal;
	std::string mnger_status;
	bool mnger_check();

public:
	void add_emp();
	void search_emp();
	void assign_mnger();
	void display_emp();
	void update_sal();
};

class member
{
	int mem_id;
	std::string mem_name;
	std::string mem_email;
	std::string mem_add1;
	std::string mem_add2;
	std::string mem_city;
	std::string mem_state;
	long long int mem_num;
	date reg_date;
	date end_date;
	std::string valid;

public:
	void add_mem();
	void refresh();
	void search_mem();
};

class sales
{
	int invoice_id;
	int sal_mem_id;
	int sal_bk_id;
	int qty;
	std::string amount;
	date date_sold;
public:
	void add_sale();
	void total_sale();
	void find_book();
};


void error()
{
	//simple error fucntion to determine if a change was made
	if (!res_set)
		std::cout << "\n\n" << "Update sucessful" << "\n\n";
	else
		std::cout << "\n\n" << "Error!! Contact Tech team" << "\n\n";

}

void connect()
{
	//a connection function to connect to the sql database
	query = stmt.str();
	Q = query.c_str();
	mysql_query(conn, Q);
}



//-------book class--------
void book::add()
{
	//function used to add books to the book table
	std::cout << "Enter the books ID: ";
	std::cin >> bk_id;
	std::cin.ignore();
	std::cout << "Enter the name of the book: ";
	getline(std::cin,bk_name);
	std::cout << "Enter the author of the book: ";
	getline(std::cin,bk_auth);
	std::cout << "Enter the price: ";
	getline(std::cin, price);
	std::cout << "Enter the amount recieved: ";
	std::cin >> qty;
	stmt.str("");
	stmt << "Insert into books values(" << bk_id <<",'" << bk_name << "', '" << bk_auth << "', '" << price << "', " << qty << "); ";
	connect();
	res_set = mysql_store_result(conn);
	error();
	mysql_free_result(res_set);

}

void book::update_price()
{
	//used to update prices of the books
	char choice;
	std::cout << "Enter the id of the book: ";
	std::cin >> bk_id;
	stmt.str("");
	stmt << "Select name, price from books where id = " << bk_id << ";";
	connect();
	res_set = mysql_store_result(conn);
	if ((row = mysql_fetch_row(res_set)) != NULL)
	{
		std::cout << "The books name is: " << row[0] << "\n";
		std::cout << "The price is: " << row[1] << "\n";
		std::cout << "Would you like to update the price? [y/n]: ";
		std::cin >> choice;
		choice = toupper(choice);

		if (choice == 'Y')
		{
			std::cout << "Enter updated price for book: ";
			std::cin >> price;
			stmt.str("");
			stmt << "Update books set price = " << price << "where id = " << bk_id << ";";
			connect();
			res_set = mysql_store_result(conn);
			error();
		}

		else
		{
			std::cout << "No changes were made\n";
		}
	}

	else
		std::cout << "No book was found\n";
	
	mysql_free_result(res_set);
}

void book::search()
{
	//made to search for books might replace with books name instead but id is safer incase two books have the same name
	std::cout << "Enter books id: ";
	std::cin >> bk_id;
	stmt.str("");
	stmt << "Select * from books where id = " << bk_id << ";";
	connect();
	res_set = mysql_store_result(conn);
	if ((row = mysql_fetch_row(res_set)) != NULL)
	{
		std::cout << "Books ID: " << row[0];
		std::cout << "\nBooks Title: " << row[1];
		std::cout << "\nBooks Author: " << row[2];
		std::cout << "\nBooks Price: " << row[3];
		std::cout << "\nBooks Remaining: " << row[4] << "\n";
	}
	else
	{
		std::cout << "No record of the book has been found!";
	}
	mysql_free_result(res_set);
}

void book::update()
{
	//uses the purchases table and book table to regester new orders and add the new books to the inventory
	//ONLY works with books that are already in the table so if a new book comes in add it through the add function and set qty to 0
	int max = 0;
	std::string b_id[100], q[100];
	stmt.str("");
	stmt << "Select book_id,qty from purchases where recieved = '" << "T" << "' and inv = 0;";
	connect();
	res_set = mysql_store_result(conn);
	stmt.str("");
	stmt << "Update purchases set inv = 1 where recieved = '" << "T" << "' and inv = 0;";
	connect();
	while ((row = mysql_fetch_row(res_set)) != NULL)
	{
		b_id[max] = row[0];
		q[max] = row[1];
		max++;
	}
	for (int i = 0; i < max; i++)//see if you can merge these two loops into one later
	{
		stmt.str("");
		stmt << "update books set qty = qty+" << q[i] << " where id = " << b_id[i] << ";";
		connect();

	}
	mysql_free_result(res_set);
	std::cout << "Update complete\n";
}

void book::display()
{
	//displays all the books and their information
	query = "Select * from books;";
	Q = query.c_str();
	mysql_query(conn, Q);
	res_set = mysql_store_result(conn);
	while ((row = mysql_fetch_row(res_set)) != NULL)
	{
		std::cout << "Books ID: " << row[0];
		std::cout << "\nBooks Title: " << row[1];
		std::cout << "\nBooks Author: " << row[2];
		std::cout << "\nBooks price: " << row[3];
		std::cout << "\nBooks remaining: " << row[4] << "\n\n\n";
	}
	mysql_free_result(res_set);
}

//----end of book class----

//---------begin of supplier class ---------

void supplier::add_sup()
{
	//adds a supplier to the supplier table
	std::cout << "Ener supplier ID: ";
	std::cin >> sup_id;
	std::cout << "Enter Supplier Name: ";
	std::cin.ignore();
	getline(std::cin,sup_name);
	std::cout << "Enter email: ";
	getline(std::cin, sup_email);
	std::cout << "Enter Phone number: ";
	std::cin >> sup_num;
	std::cin.ignore();
	std::cout << "Address 1: ";
	getline(std::cin, sup_address1);
	std::cout << "Address 2: ";
	getline(std::cin, sup_address2);
	std::cout << "City: ";
	getline(std::cin, sup_city);
	std::cout << "State: ";
	getline(std::cin, sup_state);
	stmt.str("");
	stmt << "Insert into suppliers(id,name,email,phone_no,addr1,addr2,addr_city,addr_stat) values(" << sup_id << ",'" << sup_name << "','" <<sup_email <<"'," << sup_num << ",'" << sup_address1 << "','" << sup_address2 << "','" << sup_city << "','" << sup_state << "');";
	connect();
	res_set = mysql_store_result(conn);
	error();
	mysql_free_result(res_set);
}

void supplier::rem_sup()
{
	//remove supplier from supplier table
	std::cout << "Enter suppliers id: ";
	std::cin >> sup_id;
	stmt.str("");
	stmt << "Delete from suppliers where id = " << sup_id << ";";
	connect();
	std::cout << "supplier succesfully removed\n";
}

void supplier::search_sup_id()
{
	//search for certain supplier might also change it to name but id is safer
	std::cout << "Enter the id for the supplier you are looking for: ";
	std::cin >> sup_id;
	stmt.str("");
	stmt << "Select * from suppliers where id = " << sup_id << ";";
	connect();
	res_set = mysql_store_result(conn);
	if ((row = mysql_fetch_row(res_set)) != NULL)
	{
		std::cout << "Suppliers ID: " << row[0] << "\n";
		std::cout << "Suppliers name: " << row[1] << "\n";
		std::cout << "Suppliers email: " << row[2] << "\n";
		std::cout << "Suppliers phone number: " << row[3] << "\n";
		std::cout << "Suppliers address1: " << row[4] << "\n";
		std::cout << "Suppliers address2: " << row[5] << "\n";
		std::cout << "Suppliers City: " << row[6] << "\n";
		std::cout << "Supplier State: " << row[7] << "\n";
	}
	else
	{
		std::cout << "supplier wasn't found\n";
	}
	mysql_free_result(res_set);
}

//----------end of supplier class---------

//---------begin of purchase class--------

void purchase::new_order()
{
	//adds a new order to the purchases table
	std::cout << "Enter order ID: ";
	std::cin >> ord_id;
	std::cout << "Enter boook ID: ";
	std::cin >> book_id;
	std::cout << "Enter the supplier ID: ";
	std::cin >> supplier_id;
	std::cout << "Enter Quantity: ";
	std::cin >> qty;
	std::cout << "Enter ETA(days): ";
	std::cin >> eta;
	stmt.str("");
	//this stmt might be a problem later when testing seems to be missing a parathesis
	stmt << "Insert into purchases values(" <<ord_id << "," << book_id << "," << supplier_id << "," << qty << ",curdate(),Date_add(curdate(), INTERVAL " << eta << " DAY)" << ",'" << "F" << "', " << NULL << "); ";
	connect();
	res_set = mysql_store_result(conn);
	error();
	std::cout << "New order has been added\n";
}

void purchase::view()
{
	//view all order of a certain type
	int choice = 0;
	while (choice <= 0 || choice > 3)
	{
		std::cout << "Select and option : \n";
		std::cout << "1. View orders not recieved\n";
		std::cout << "2. View orders recieved\n";
		std::cout << "3. View cancelled orders\n";
		std::cin >> choice;
		stmt.str("");
		//might have a problem need to check how to check how something is false in SQL
		switch (choice)
		{
			case 1:
				received = 'F';
				break;
			case 2:
				received = 'T';
				break;
			case 3:
				received = 'C';
				break;
			default:
				std::cout << "value choosen was not valid please choose again\n";
		}
	}
	stmt << "Select * from purchases where recieved = '" << received << "';";
	connect();
	res_set = mysql_store_result(conn);
	while ((row = mysql_fetch_row(res_set)) != NULL)
	{
		std::cout << "Order ID: " << row[0] << "\n";
		std::cout << "Book ID: " << row[1] << "\n";
		std::cout << "Supplier ID: " << row[2] << "\n";
		std::cout << "Quantity: " << row[3] << "\n";
		std::cout << "Date Ordered: " << row[4] << "\n";
		std::cout << "Estimated Delivery Date: " << row[5] << "\n";
		std::cout << "\n\n\n";
	}
	mysql_free_result(res_set);
}

void purchase::mark_cancel()
{
	//mark an order as cancelled
	std::cout << "Enter order ID for cancellation: ";
	std::cin >> ord_id;
	stmt.str("");
	//the extra bit at the end prevents the user from marking recieved orders as cancelled
	stmt << "Update purchases set recieved = '" << "C" << "' where ord_id = " << ord_id << " AND recieved != '" << "T" << "'; ";
	connect();
	std::cout << "Order successfully cancelled\n";
}

void purchase::mark_recive()
{
	//mark order as recieved
	std::cout << "Enter order ID to mark recieved: ";
	std::cin >> ord_id;
	stmt.str("");
	stmt << "Update purchases set recieved = '" << "T" << "' where ord_id = " << ord_id << "; ";
	connect();
	std::cout << "Order successfully recieved\n";
}

//-------end of purchase class--------

//-------begin of employee class------
//in order to use this class you have to assign a manager in the sql table as there is no way of making the
//first manager here

void employee::add_emp()
{
	//adds employee to employee table
	if(mnger_check())
	{
		std::cout << "Enter details of new employee\n";
		std::cout << "Enter ID: ";
		std::cin >> emp_id;
		std::cout << "Enter name: ";
		std::cin.ignore();
		getline(std::cin, emp_name);
		std::cout << "Enter email: ";
		getline(std::cin, emp_email);
		std::cout << "Enter phone number: ";
		std::cin >> emp_num;
		std::cout << "Enter address1: ";
		std::cin.ignore();
		getline(std::cin, emp_addres1);
		std::cout << "Enter address2: ";
		getline(std::cin, emp_addres2);
		std::cout << "Enter city: ";
		getline(std::cin, city);
		std::cout << "Enter state: ";
		getline(std::cin, state);
		std::cout << "Enter salary: ";
		getline(std::cin, sal);
		stmt.str("");
		stmt << "Insert into employees values(" << emp_id << ",'" << emp_name << "','" << emp_email << "'," << emp_num << ",'" << emp_addres1 << "','" << emp_addres2 << "','" << city << "','" << state << "',curdate()," << sal << "," << 0 << "); ";
		connect();
		std::cout << "\nEmployee has been added\n";
	}
	mysql_free_result(res_set);
}

void employee::search_emp()
{
	//search for employee through id
	std::cout << "Enter the ID of the employee you are looking for: ";
	std::cin >> emp_id;
	stmt.str("");
	stmt << "Select * from employees where id = " << emp_id << ";";
	connect();
	res_set = mysql_store_result(conn);
	if ((row = mysql_fetch_row(res_set)) != NULL)
	{
		std::cout << "Employee Details\n";
		std::cout << "Employee ID: " << row[0] << "\n";
		std::cout << "Employee name: " << row[1] << "\n";
		std::cout << "Employee email: " << row[2] << "\n";
		std::cout << "Employee number: " << row[3] << "\n";
		std::cout << "Employee address1: " << row[4] << "\n";
		std::cout << "Employee address2: " << row[5] << "\n";
		std::cout << "Employee city: " << row[6] << "\n";
		std::cout << "Employee state: " << row[7] << "\n";
		std::cout << "Employee hiring date: " << row[8] << "\n";
		std::cout << "Employee salary: " << row[9] << "\n";
	}
	else
	{
		std::cout << "\nEmployee not found\n";
	}

	mysql_free_result(res_set);
}

void employee::assign_mnger()
{
	//grants managers privileges to employees
	if(mnger_check())
	{
		std::cout << "Enter employee ID for promotion: ";
		std::cin >> emp_id;
		stmt.str("");
		//this may cause problems need to look how to update bool on a table through commands
		stmt << "update employees set manger_stat = 1 where id = " << emp_id << ";";
		connect();
		std::cout << "\n\n\nMANGERS PRIVILEGES\n\n\n";
	}
	mysql_free_result(res_set);
}

void employee::display_emp()
{
	//displays all information on all employees
	query = "SELECT * FROM employees";
	Q = query.c_str();
	mysql_query(conn, Q);
	res_set = mysql_store_result(conn);
	while ((row = mysql_fetch_row(res_set)) != NULL)
	{
		std::cout << "Employee Details\n";
		std::cout << "Employee ID: " << row[0] << "\n";
		std::cout << "Employee name: " << row[1] << "\n";
		std::cout << "Employee email: " << row[2] << "\n";
		std::cout << "Employee number: " << row[3] << "\n";
		std::cout << "Employee address1: " << row[4] << "\n";
		std::cout << "Employee address2: " << row[5] << "\n";
		std::cout << "Employee city: " << row[6] << "\n";
		std::cout << "Employee state: " << row[7] << "\n";
		std::cout << "Employee hiring date: " << row[8] << "\n";
		std::cout << "Employee salary: " << row[9] << "\n";
		std::cout << "\n\n\n\n";

	}
	mysql_free_result(res_set);

}

void employee::update_sal()
{
	//updates employees salary
	if(mnger_check())
	{
		std::cout << "Enter the employees ID: ";
		std::cin >> emp_id;
		std::cout << "Enter the new salary value: ";
		std::cin >> sal;
		stmt.str("");
		stmt << "Update employees set salary = " << sal << "where id = " << emp_id << ";";
		connect();
		std::cout << "\n\nSalary Updated\n\n";
	}
	mysql_free_result(res_set);

}

bool employee::mnger_check()
{
	//used to determine if the employee trying to make the change is a manager
	std::cout << "Enter your ID for verification: ";
	std::cin >> emp_id;
	stmt.str("");
	stmt << "Select manger_stat from employees where id = " << emp_id << ";";
	connect();
	res_set = mysql_store_result(conn);
	row = mysql_fetch_row(res_set);
	if (row == NULL)
	{
		std::cout << "\nEmployee not found\n";
		return false;
	}
	else
	{
		mnger_status = row[0];
		if (mnger_status == "0")
		{
			std::cout << "Acess Denied please report to managment\n";
			return false;
		}
	}

	return true;
}

//---------end of employee class------

//---------begin of member class------

void member::add_mem()
{
	//add a member to the member table
	std::cout << "Enter new member ID: ";
	std::cin >> mem_id;
	std::cout << "Enter member name: ";
	std::cin.ignore();
	getline(std::cin, mem_name);
	std::cout << "Enter member email: ";
	getline(std::cin, mem_email);
	std::cout << "Enter member address: ";
	getline(std::cin, mem_add1);
	std::cout << "Enter second address: ";
	getline(std::cin, mem_add2);
	std::cout << "Enter city: ";
	getline(std::cin,mem_city);
	std::cout << "Enter state: ";
	getline(std::cin, mem_state);
	std::cout << "Enter phone number: ";
	std::cin >> mem_num;
	stmt.str("");
	stmt << "Insert into members(id, name, email, address1, address2, city, state, phone_number, beg_date, end_date, valid) values("<< mem_id << ",'" << mem_name << "','" << mem_email << "','" << mem_add1 << "','"<< mem_add2 << "','"  << mem_city << "','" << mem_state << "'," << mem_num << ",curdate(),Date_add(curdate(), INTERVAL 1 YEAR)" << ",'" << "T"  << "');";
	connect();
	std::cout << "\n\n\nMEMBER ADDED\n\n\n";
}

void member::refresh()
{
	//this goes off everytime the member menu is opened and refreshes all members whose membership has expired
	stmt.str("");
	stmt << "Update members set valid = '" << "F" << "' where end_date <= curdate(); ";
	connect();
}

void member::search_mem()
{
	//Searches for a memeber and displays all information
	std::cout << "Enter the members ID: ";
	std::cin >> mem_id;
	stmt.str("");
	stmt << "Select * from members where id = " << mem_id << ";";
	connect();
	res_set = mysql_store_result(conn);
	if ((row = mysql_fetch_row(res_set)) != NULL)
	{
		std::cout << "Member Details" << "\n";
		std::cout << "ID: " << row[0] << "\n";
		std::cout << "Name: " << row[1] << "\n";
		std::cout << "Email: " << row[2] << "\n";
		std::cout << "Address: " << row[3] << "\n";
		std::cout << "Address2: " << row[4] << "\n";
		std::cout << "City: " << row[5] << "\n";
		std::cout << "State: " << row[6] << "\n";
		std::cout << "Phone number: " << row[7] << "\n";
		std::cout << "Begin Date: " << row[8] << "\n";
		std::cout << "End Date: " << row[9] << "\n";
	}
	else
	{
		std::cout << "\n\nMember doesn't exist\n\n";
	}
	mysql_free_result(res_set);
}

//---------end of member class--------

//---------begin of sales class-------
void sales::add_sale()
{
	//adds a sale to the sales table
	find_book();
	mysql_free_result(res_set);	
	std::cout << "Please enter invoice ID: ";
	std::cin >> invoice_id;
	stmt.str("");
	stmt << "Insert into sales(inv_id, mem_id, book_id, qty, amount, date_sold) values(" << invoice_id << "," << sal_mem_id << "," << sal_bk_id << "," << qty << "," << amount << ",curdate());";
	connect();

}

void sales::total_sale()
{
	//calculates all sales for the year
	query = "Select sum(amount) from sales where year(date_sold) = year(curdate());";
	Q = query.c_str();
	mysql_query(conn, Q);
	res_set = mysql_store_result(conn);
	if ((row = mysql_fetch_row(res_set)) != NULL)
	{
		std::cout << "\n\nTotal sales this year: " << row[0] << "\n\n";
	}
	mysql_free_result(res_set);
}

void sales::find_book()
{
	//adjust books inventory and calculates the total cost of the transation
	std::cout << "Enter member ID: ";
	std::cin >> sal_mem_id;
	std::cout << "Enter book ID: ";
	std::cin >> sal_bk_id;
	std::cout << "Enter quantity: ";
	std::cin >> qty;
	stmt.str("");
	stmt.str("");
	stmt << "update books set qty = qty-" << qty << " where id = " << sal_bk_id << ";";
	connect();
	stmt.str("");
	stmt << "Select price*" << qty << " from books where id = " << sal_bk_id << ";";
	connect();
	res_set = mysql_store_result(conn);	
	if ((row = mysql_fetch_row(res_set)) != NULL)
	{
		std::cout << "The bill amount: " << row[0] << "\n";
		amount = row[0];
	}
	else
	{
		std::cout << "\n\nBook ID invalid\n\n";
	}

	return;
}

//---------end of sales calss---------

//---------Menu functions-------------
void book_menu();
void sup_menu();
void pur_menu();
void emp_menu();
void mem_menu();
void sal_menu();

void main_menu()
{
	std::cout << "=====================================\n";
	std::cout << "       BOOKSHOP MANEGMENT SYSTEM     \n";
	std::cout << "=====================================\n";
	std::cout << "1)  BOOKS\n";
	std::cout << "2)  SUPPLIERS\n";
	std::cout << "3)  PURCHASES\n";
	std::cout << "4)  EMPLOYEES\n";
	std::cout << "5)  MEMBERS\n";
	std::cout << "6)  SALES\n";
	std::cout << "7)  EXIT\n";
	int x;
	std::cout << "Please choose an option: ";
	std::cin >> x;
	switch (x)
	{
		case 1:
			book_menu();
			_getch();
			break;
		
		case 2:
			system("cls");
			sup_menu();
			_getch();
			break;
		
		case 3:
			system("cls");
			pur_menu();
			_getch();
			break;

		case 4:
			system("cls");
			emp_menu();
			_getch();
			break;

		case 5:
			system("cls");
			mem_menu();
			_getch();
			break;

		case 6:
			system("cls");
			sal_menu();
			_getch();
			break;

		case 7:
			exit(1);

		default:
			system("cls");
			std::cout << "\nInvalid Input\n";
			_getch();
	}
}

void book_menu()
{
	std::cout << "=====================================\n";
	std::cout << "              BOOK MENU			   \n";
	std::cout << "=====================================\n";
	std::cout << "1)  ADD\n";
	std::cout << "2)  UPDATE PRICES\n";
	std::cout << "3)  SEARCH\n";
	std::cout << "4)  UPDATE\n";
	std::cout << "5)  DISPLAY\n";
	std::cout << "6)  RETURN TO MAIN MENU\n";
	int x;
	book b;
	std::cout << "Choose an option: ";
	std::cin >> x;
	switch (x)
	{
	case 1:
		system("cls");
		b.add();
		break;

	case 2:
		system("cls");
		b.update_price();
		break;

	case 3:
		system("cls");
		b.search();
		break;

	case 4:
		system("cls");
		b.update();
		break;

	case 5:
		system("cls");
		b.display();
		break;

	case 6:
		system("cls");
		break;

	default:
		system("cls");
		std::cout << "Invlaid Input\n";
		_getch();
	}

}

void sup_menu()
{
	std::cout << "=====================================\n";
	std::cout << "            SUPPLIER MENU		       \n";
	std::cout << "=====================================\n";
	std::cout << "1)  ADD\n";
	std::cout << "2)  REMOVE\n";
	std::cout << "3)  SEARCH\n";
	std::cout << "4)  RETURN TO MAIN MENU\n";
	int x;
	supplier s;
	std::cout << "Enter your choice: ";
	std::cin >> x;
	switch (x)
	{
		case 1:
			system("cls");
			s.add_sup();
			break;

		case 2:
			system("cls");
			s.rem_sup();
			break;

		case 3:
			system("cls");
			s.search_sup_id();
			break;

		case 4:
			return;

		default:
			system("cls");
			std::cout << "Invalid Input\n";
			_getch();
	}
}

void pur_menu()
{
	std::cout << "=====================================\n";
	std::cout << "            PURCHASE MENU		       \n";
	std::cout << "=====================================\n";
	std::cout << "1)  NEW ORDER\n";
	std::cout << "2)  VIEW ALL\n";
	std::cout << "3)  CANCEL ORDER\n";
	std::cout << "4)  RECIEVED ORDER\n";
	std::cout << "5)  RETURN TO MAIN MENU\n";
	int x;
	purchase p;
	std::cout << "Enter your choice: ";
	std::cin >> x;
	switch(x)
	{
		case 1:
			system("cls");
			p.new_order();
			break;

		case 2:
			system("cls");
			p.view();
			break;

		case 3:
			system("cls");
			p.mark_cancel();
			break;

		case 4:
			system("cls");
			p.mark_recive();
			break;

		case 5:
			system("cls");
			break;

		default:
			system("cls");
			std::cout << "INVALID INPUT\n";
			_getch();
	}
}

void emp_menu()
{
	std::cout << "=====================================\n";
	std::cout << "            EMPLOYEE MENU		       \n";
	std::cout << "=====================================\n";
	std::cout << "1) ADD AN EMPLOYEE\n";
	std::cout << "2) SEARCH FOR AN EMPLOYEE\n";
	std::cout << "3) ASSIGN MANAGER\n";
	std::cout << "4) DISPLAY ALL EMPLOYEE\n";
	std::cout << "5) UPDATE SALARY\n";
	std::cout << "6) RETURN TO MAIN MENU\n";
	int x;
	employee e;
	std::cout << "Enter Your Choice: ";
	std::cin >> x;
	switch(x)
	{
	case 1:
		system("cls");
		e.add_emp();
		break;

	case 2:
		system("cls");
		e.search_emp();
		break;

	case 3:
		system("cls");
		e.assign_mnger();
		break;

	case 4:
		system("cls");
		e.display_emp();
		break;

	case 5:
		system("cls");
		e.update_sal();
		break;

	case 6:
		system("cls");
		break;

	default:
		system("cls");
		std::cout << "INVLID INPUT";
		_getch();
	}
}

void mem_menu()
{
	std::cout << "=====================================\n";
	std::cout << "            MEMEBER MENU		       \n";
	std::cout << "=====================================\n";
	std::cout << "1) ADD A NEW MEMBER\n";
	std::cout << "2) SEARCH FOR MEMBER\n";
	std::cout << "3) RETURN TO MAIN MENU\n";
	int x;
	member m;
	m.refresh();
	std::cout << "Enter your Choice: ";
	std::cin >> x;
	switch (x)
	{
		case 1:
			system("cls");
			m.add_mem();
			break;

		case 2:
			system("cls");
			m.search_mem();
			break;

		case 3:
			system("cls");
			break;

		default:
			system("cls");
			std::cout << "INVLAID INPUT";
			_getch();
	}
}

void sal_menu()
{
	std::cout << "=====================================\n";
	std::cout << "            SALES MENU		       \n";
	std::cout << "=====================================\n";
	std::cout << "1) ADD A NEW BILL\n";
	std::cout << "2) SALES FOR THE YEAR\n";
	std::cout << "3) RETURN TO MAIN MENU\n";
	int x;
	sales s;
	std::cout << "Enter your choice: ";
	std::cin >> x;
	switch (x)
	{
		case 1:
			system("cls");
			s.add_sale();
			break;

		case 2:
			system("cls");
			s.total_sale();
			break;

		case 3:
			system("cls");
			break;

		default:
			system("cls");
			std::cout << "INVALID INPUT\n";
			_getch();
	}
}
//---------end of menu fucntions-----


void pass()
{
	//simple password funciton to act as a login to use the terminal if you want this off you can just comment out the function in main
	int num = 0;
	std::cout << "Enter password: ";
	for (int i = 0; i < 4; i++)
	{
		num = num * 10 + (_getch() - 48);
		std::cout << "*";
	}
	if (num == PASSWORD)
	{
		std::cout << "\nLOGIN SUCESSFUL PRESS ANY KEY CONTINUE\n";
		_getch();
	}
	else
	{
		std::cout << "\nINCORRECT PASSWORD\n";
		_getch();
		exit(1);
	}
}


int main()
{
	pass();
	conn = mysql_init(0);
	conn = mysql_real_connect(conn , HOST, USER, PASS, DATABASE, PORT, NULL, 0);
	if (conn)
	{
		while (true)
		{
			system("cls");
			main_menu();
		}
	}
	else
	{
		system("cls");
		std::cout << "ERROR WITH CONNECTION TO DATABASE\n";
		_getch();
	}
	return 0;
}

