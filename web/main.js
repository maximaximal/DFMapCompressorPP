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
