/* $Id: managers_bar.js,v 1.1.2.6 2010/01/05 14:17:26 danillonunes Exp $ */

Drupal.behaviors.managersBar = function(context) {
  var $document = $('body', context);
  var $managersBar = $('#managers-bar', context);
  
  if ($managersBar.length) {
    var managersBarHeight = $managersBar.height();
    var documentHeight = parseInt($document.css('margin-top')) + managersBarHeight;
    
    $document.css('margin-top', documentHeight);
  }
}