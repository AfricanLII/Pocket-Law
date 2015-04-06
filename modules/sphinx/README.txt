

README.txt

Installation instructions:

You need to edit sphinx.install to reflect the location of your Sphinx server.  Similarly, you will want to set values for SPH_DEFAULT_HOST and SPH_DEFAULT_PORT 
at the top of the sphinx.module file.  This is clumsy, but better than the original version, in which localhost was assumed as the default
with no real way to change it.

To test for firewall or other problems connecting with the server, use test.php in the ./test subdirectory.