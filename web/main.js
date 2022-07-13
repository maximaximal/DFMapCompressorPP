/*
* FileSaver.js
* A saveAs() FileSaver implementation.
*
* By Eli Grey, http://eligrey.com
*
* License : https://github.com/eligrey/FileSaver.js/blob/master/LICENSE.md (MIT)
* source  : http://purl.eligrey.com/github/FileSaver.js
*/

// The one and only way of getting global scope in all environments
// https://stackoverflow.com/q/3277182/1008999

var _global = typeof window === 'object' && window.window === window
  ? window : typeof self === 'object' && self.self === self
  ? self : typeof global === 'object' && global.global === global
  ? global
  : this

function bom (blob, opts) {
  if (typeof opts === 'undefined') opts = { autoBom: false }
  else if (typeof opts !== 'object') {
    console.warn('Deprecated: Expected third argument to be a object')
    opts = { autoBom: !opts }
  }

  // prepend BOM for UTF-8 XML and text/* types (including HTML)
  // note: your browser will automatically convert UTF-16 U+FEFF to EF BB BF
  if (opts.autoBom && /^\s*(?:text\/\S*|application\/xml|\S*\/\S*\+xml)\s*;.*charset\s*=\s*utf-8/i.test(blob.type)) {
    return new Blob([String.fromCharCode(0xFEFF), blob], { type: blob.type })
  }
  return blob
}

function download (url, name, opts) {
  var xhr = new XMLHttpRequest()
  xhr.open('GET', url)
  xhr.responseType = 'blob'
  xhr.onload = function () {
    saveAs(xhr.response, name, opts)
  }
  xhr.onerror = function () {
    console.error('could not download file')
  }
  xhr.send()
}

function corsEnabled (url) {
  var xhr = new XMLHttpRequest()
  // use sync to avoid popup blocker
  xhr.open('HEAD', url, false)
  try {
    xhr.send()
  } catch (e) {}
  return xhr.status >= 200 && xhr.status <= 299
}

// `a.click()` doesn't work for all browsers (#465)
function click (node) {
  try {
    node.dispatchEvent(new MouseEvent('click'))
  } catch (e) {
    var evt = document.createEvent('MouseEvents')
    evt.initMouseEvent('click', true, true, window, 0, 0, 0, 80,
                          20, false, false, false, false, 0, null)
    node.dispatchEvent(evt)
  }
}

// Detect WebView inside a native macOS app by ruling out all browsers
// We just need to check for 'Safari' because all other browsers (besides Firefox) include that too
// https://www.whatismybrowser.com/guides/the-latest-user-agent/macos
var isMacOSWebView = _global.navigator && /Macintosh/.test(navigator.userAgent) && /AppleWebKit/.test(navigator.userAgent) && !/Safari/.test(navigator.userAgent)

var saveAs = _global.saveAs || (
  // probably in some web worker
  (typeof window !== 'object' || window !== _global)
    ? function saveAs () { /* noop */ }

  // Use download attribute first if possible (#193 Lumia mobile) unless this is a macOS WebView
  : ('download' in HTMLAnchorElement.prototype && !isMacOSWebView)
  ? function saveAs (blob, name, opts) {
    var URL = _global.URL || _global.webkitURL
    // Namespace is used to prevent conflict w/ Chrome Poper Blocker extension (Issue #561)
    var a = document.createElementNS('http://www.w3.org/1999/xhtml', 'a')
    name = name || blob.name || 'download'

    a.download = name
    a.rel = 'noopener' // tabnabbing

    // TODO: detect chrome extensions & packaged apps
    // a.target = '_blank'

    if (typeof blob === 'string') {
      // Support regular links
      a.href = blob
      if (a.origin !== location.origin) {
        corsEnabled(a.href)
          ? download(blob, name, opts)
          : click(a, a.target = '_blank')
      } else {
        click(a)
      }
    } else {
      // Support blobs
      a.href = URL.createObjectURL(blob)
      setTimeout(function () { URL.revokeObjectURL(a.href) }, 4E4) // 40s
      setTimeout(function () { click(a) }, 0)
    }
  }

  // Use msSaveOrOpenBlob as a second approach
  : 'msSaveOrOpenBlob' in navigator
  ? function saveAs (blob, name, opts) {
    name = name || blob.name || 'download'

    if (typeof blob === 'string') {
      if (corsEnabled(blob)) {
        download(blob, name, opts)
      } else {
        var a = document.createElement('a')
        a.href = blob
        a.target = '_blank'
        setTimeout(function () { click(a) })
      }
    } else {
      navigator.msSaveOrOpenBlob(bom(blob, opts), name)
    }
  }

  // Fallback to using FileReader and a popup
  : function saveAs (blob, name, opts, popup) {
    // Open a popup immediately do go around popup blocker
    // Mostly only available on user interaction and the fileReader is async so...
    popup = popup || open('', '_blank')
    if (popup) {
      popup.document.title =
      popup.document.body.innerText = 'downloading...'
    }

    if (typeof blob === 'string') return download(blob, name, opts)

    var force = blob.type === 'application/octet-stream'
    var isSafari = /constructor/i.test(_global.HTMLElement) || _global.safari
    var isChromeIOS = /CriOS\/[\d]+/.test(navigator.userAgent)

    if ((isChromeIOS || (force && isSafari) || isMacOSWebView) && typeof FileReader !== 'undefined') {
      // Safari doesn't allow downloading of blob URLs
      var reader = new FileReader()
      reader.onloadend = function () {
        var url = reader.result
        url = isChromeIOS ? url : url.replace(/^data:[^;]*;/, 'data:attachment/file;')
        if (popup) popup.location.href = url
        else location = url
        popup = null // reverse-tabnabbing #460
      }
      reader.readAsDataURL(blob)
    } else {
      var URL = _global.URL || _global.webkitURL
      var url = URL.createObjectURL(blob)
      if (popup) popup.location = url
      else location.href = url
      popup = null // reverse-tabnabbing #460
      setTimeout(function () { URL.revokeObjectURL(url) }, 4E4) // 40s
    }
  }
)

_global.saveAs = saveAs.saveAs = saveAs

// End of FileSaver

const dfmapcompressorpp = new Worker('worker.js');

let compressPromiseResolve = null;
let compressPromiseReject = null;

function setReady() {
    let button = document.getElementById("button-run");
    button.disabled = false;
    button.innerText = "Generate .fdf-map!";
    button.classList.remove('btn-outline-primary');
    button.classList.add('btn-primary');

    button.addEventListener('click', async function(e) {

    });
}

const inputWidth = document.getElementById('input-tilewidth')
const inputHeight = document.getElementById('input-tileheight')
const alertPlaceholder = document.getElementById('alert-placeholder')
const alert = (message, type) => {
    const wrapper = document.createElement('div')
    wrapper.innerHTML = [
	`<div class="alert alert-${type} alert-dismissible" role="alert">`,
	`   <div>${message}</div>`,
	'   <button type="button" class="btn-close" data-bs-dismiss="alert" aria-label="Close"></button>',
	'</div>'
    ].join('')

    alertPlaceholder.append(wrapper)
}

dfmapcompressorpp.onmessage = msg => {
    switch(msg.data.type) {
    case 'ready':
	setReady();
	break;
    case 'progress':
	console.log(msg.data.progress);
	break;
    case 'result':
	console.log(msg.data.name);
	const b = new Blob([msg.data.data], {type: "application/octet-stream"});
	saveAs(b, msg.data.name);
	break;
    }
}

const compress = async function(tileWidth, tileHeight, files) {
    let compressPromise = new Promise(function(resolve, reject) {
	compressPromiseResolve = resolve;
	compressPromiseReject = reject;
    });
    dfmapcompressorpp.postMessage({'tileWidth': tileWidth, 'tileHeight': tileHeight, 'files': files});
    return await compressPromise;
}

let filelist = document.getElementById("filelist");

// https://stackoverflow.com/a/53058574
// Drop handler function to get all files
async function getAllFileEntries(dataTransferItemList) {
  let fileEntries = [];
  // Use BFS to traverse entire directory/file structure
  let queue = [];
  // Unfortunately dataTransferItemList is not iterable i.e. no forEach
  for (let i = 0; i < dataTransferItemList.length; i++) {
    queue.push(dataTransferItemList[i].webkitGetAsEntry());
  }
  while (queue.length > 0) {
    let entry = queue.shift();
    if (entry.isFile) {
      fileEntries.push(entry);
    } else if (entry.isDirectory) {
      let reader = entry.createReader();
      queue.push(...await readAllDirectoryEntries(reader));
    }
  }
  return fileEntries;
}

// Get all the entries (files or sub-directories) in a directory by calling
// readEntries until it returns empty array
async function readAllDirectoryEntries(directoryReader) {
  let entries = [];
  let readEntries = await readEntriesPromise(directoryReader);
  while (readEntries.length > 0) {
    entries.push(...readEntries);
    readEntries = await readEntriesPromise(directoryReader);
  }
  return entries;
}

// Wrap readEntries in a promise to make working with readEntries easier
async function readEntriesPromise(directoryReader) {
  try {
    return await new Promise((resolve, reject) => {
      directoryReader.readEntries(resolve, reject);
    });
  } catch (err) {
    console.log(err);
  }
}

var elDrop = document.getElementById('dropzone');

elDrop.addEventListener('dragover', function (event) {
    event.preventDefault();

    while (filelist.filelist) {
	filelist.removeChild(filelist.lastChild);
    }
});

const collator = new Intl.Collator('en', { numeric: true, sensitivity: 'base' });
elDrop.addEventListener('drop', async function (event) {
    event.preventDefault();
    let items = await getAllFileEntries(event.dataTransfer.items);

    items.sort(function(a, b) {
	return collator.compare(a.name, b.name);
    });

    while (filelist.filelist) {
	filelist.removeChild(filelist.lastChild);
    }

    let objs = [];
    for(const e of items) {
	if(!e.name.endsWith('bmp')) continue;
	let entry = document.createElement("li");
	entry.classList.add('list-group-item');
	entry.innerText = e.name;
	filelist.appendChild(entry);
	let promise = new Promise((resolve, reject) => {
	    e.file(resolve, reject);
	});
	objs.push(promise);
    }
    if(objs.length == 0) {
	alert("No valid files! Please only drop <code>.bmp</code> files containing tile-map data.", 'danger');
	return;
    }
    let files = [];
    for(let i = 0; i < objs.length; ++i) {
	let o = objs[i];
	files.push(o.then(v => objs[i] = v));
    }
    await Promise.all(objs);
    await Promise.all(files);
    compress(parseInt(inputWidth.value), parseInt(inputHeight.value), objs);
});
