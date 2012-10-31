var skip = 0;

function ad_go() 
{
  if (skip) 
  {
     skip = 0
     return
  }
  
  if (navigator.appName == 'Microsoft Internet Explorer') 
  {
    sel = document.selection.createRange();
    if(document.selection.type == 'Text' && sel.text != '') 
    {
      document.selection.empty();
      goto_selection(sel.text);
    }
  }
  else
  {
    goto_selection(document.getSelection());
  }
}

function goto_selection(word) 
{
   while (word.substr(word.length - 1, 1)==' ') 
	 word = word.substr(0, word.length-1)
   while (word.substr(0,1)==' ') 
	 word=word.substr(1)
   if (word != '') 
   {
      document.location = "/ad.dic?F=3&W=" + word;
   }
}

function ad_search()
{
 keywords=document.forms["adform"].elements["W"].value;
 keywords=keywords.replace(/"/gi,'/');
 keywords=keywords.replace(/^\//gi,'');
 keywords=keywords.replace(/\/$/gi,'');
 keywords=keywords.replace(/\/ | \//gi,'/');
 keywords=keywords.replace(/\/{2}/gi,'/');
 keywords=keywords.replace(/ /gi,'_');
 keywords=keywords.replace(/\./gi,'_');
 keywords=keywords.replace(/__/gi,'_');
 keywords=keywords.replace(/_*$/gi,'');
 keywords=keywords.replace(/^_*/gi,'');
 keywords=keywords.toLowerCase();
 if (keywords!='')
 {
	document.location='/ad.dic?F=3&W='+keywords;
 }
}
