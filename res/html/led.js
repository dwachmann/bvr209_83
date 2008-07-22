/**
 *
 */
function startLED(ev)
{ if( typeof(ev)=='undefined' )
    ev = window.event;
  
  var led = document.getElementById('led');

  if( led )
    led.Run = true;
} // of function startLED()

/**
 *
 */
function stopLED(ev)
{ if( typeof(ev)=='undefined' )
    ev = window.event;

  var led = document.getElementById('led');

  if( led )
    led.Run = false;
} // of stopLED()

/**
 *
 */
function ledAbout(ev)
{ if( typeof(ev)=='undefined' )
    ev = window.event;

  var led = document.getElementById('led');

  if( led )
    led.AboutBox();
} // of ledAbout()

/**
 *
 */
function ledPropertyPages(ev)
{ if( typeof(ev)=='undefined' )
    ev = window.event;

  var led = document.getElementById('led');

  if( led )
    led.PropertyPages();
} // of ledPropertyPages()

/**
 *
 */
function ledSetText(ev)
{ if( typeof(ev)=='undefined' )
    ev = window.event;

  var led = document.getElementById('led');
  var t   = document.getElementById('ledtext');
  
  if( led && t )
    led.Text = t.value;
} // of ledSetText()

/**
 *
 */
function ledGetText(ev)
{ if( typeof(ev)=='undefined' )
    ev = window.event;

  var led = document.getElementById('led');
  var t   = document.getElementById('ledtext');
  
  if( led && t )
    t.value = led.Text;
} // of ledGetText()

/**
 *
 */
function ledClockChanged(ev)
{ if( typeof(ev)=='undefined' )
    ev = window.event;

  var led = document.getElementById('led');
  
  if( led )
    led.Clock = ev.srcElement.checked;
} // of ledClockChanged()

/**
 *
 */
function led24HourChanged(ev)
{ if( typeof(ev)=='undefined' )
    ev = window.event;

  var led = document.getElementById('led');

  if( led )
    led.Display24Hour = ev.srcElement.checked;
} // of led24HourChanged()

/**
 *
 */
function led16SegmentChanged(ev)
{ if( typeof(ev)=='undefined' )
    ev = window.event;

  var led = document.getElementById('led');

  if( led )
    led.Display16Segment = ev.srcElement.checked;
} // of led16SegmentChanged()

/**
 *
 */
function ledStarted()
{ var startLED = document.getElementById('startLED');
  var stopLED  = document.getElementById('stopLED');
  
  if( startLED && stopLED )
  { startLED.disabled = true;
    stopLED.disabled  = false;
  } // of if
  
  window.status="ledStarted";
}

/**
 *
 */
function ledStopped()
{ var startLED = document.getElementById('startLED');
  var stopLED  = document.getElementById('stopLED');

  if( startLED && stopLED )
  { startLED.disabled = false;
    stopLED.disabled  = true;
  } // of if

  window.status="ledStopped";
}

var dirtyCount=0;

/**
 *
 */
function ledDirty()
{ //window.status = "ledDirty:"+dirtyCount;

  dirtyCount++;
  
  var led = document.getElementById('led');

  if( led )
    led.Refresh();
}

var propchangecount=0;
function propchanged()
{ window.status="property changed ["+propchangecount+"] "+window.event.propertyName;

  propchangecount++;
}

window.onload = function()
{ var led = document.getElementById('led');

  if( led )
  { led.attachEvent('OnStarted',ledStarted);
    led.attachEvent('OnStopped',ledStopped);
    led.attachEvent('OnDirty',ledDirty);
    
    document.getElementById('startLED').onclick=startLED;
    document.getElementById('stopLED').onclick=stopLED;

    document.getElementById('settext').onclick=ledSetText;
    document.getElementById('gettext').onclick=ledGetText;
    document.getElementById('isClock').onclick=ledClockChanged;
    document.getElementById('24Hour').onclick=led24HourChanged;
    document.getElementById('16Segment').onclick=led16SegmentChanged;
    document.getElementById('about').onclick=ledAbout;
    document.getElementById('proppage').onclick=ledPropertyPages;

    document.getElementById('isClock').checked   = led.Clock;
    document.getElementById('24Hour').checked    = led.Display24Hour;
    document.getElementById('16Segment').checked = led.Display16Segment;
  } // of if
}

