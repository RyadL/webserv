#!/usr/bin/python

# Import modules for CGI handling 
import cgi

# Create instance of FieldStorage 
form = cgi.FieldStorage() 

# Get data from fields
fname = form.getvalue('fname')
lname  = form.getvalue('lname')


print("ON: 0")
print("FAIT: 1")
print("UN: 2")
print("TEST: 3")
print("Content-type:text/html")
print("")
print("<html>")
print("<head>")
print("<title>Hello - Second CGI Program</title>")
print("</head>")
print("<body>")
print("<h2>Hello %s %s</h2>" % (fname, lname))
print("</body>")
print("</html>")