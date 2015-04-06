
if (Drupal.jsEnabled) {
  $(document).ready(function() {
    $(".tab-settings").each(function(i) {
      $(this).hide();
    });
    shareSetTabClick("div.tab-title");
  });
}

function shareSetTabClick(name) {
  $(name).each(function(i) {
    var id = $(this).parent().attr('id');
    $(this).click(function() {
      var title = $(this);
      $("#" + id + " div.tab-settings").animate({
        height: 'toggle'
      },function() {
        $(title).toggleClass('collapsed');
        $(title).toggleClass('expanded');
      });
    });
  });
}
