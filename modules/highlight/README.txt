highlight.module
README.txt

Tested on: drupal 4.7.4

Description
------------

The highlight module highlights terms on a node's page.  It's probably
most useful for highlighting search keywords in pages. 

Usage
-----

1) activate "highlight" module in admin > modules
2) add the filter to input formats admin > input formats
3) customize highlighting admin > settings > highlight (optional)
4) customize css for (optional)

Once the module is enabled, it will highlight keywords 
passed into node pages.  To pass the keywords in, append them to the
url as a comma-separated list in the form

&highlight=first,second,third

An example url might look like

http://www.example.org/?q=node/34&highlight=first,second

Additionally, the module will pick up search terms from 
search/node/myTerm and highlight instances of myTerm on the 
node page.

Questions? Contact arthur @ civicactions . com