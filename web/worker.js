importScripts('./dfmapcompressorpp.js');

let progresscb = (p) => postMessage({'type': 'progress', 'progress': p});

Module().then(function(m) {
    const FS = m.FS;
    const WORKERFS = m.WORKERFS;
    onmessage = msg => {
	msg = msg.data;
	let l = new m.StringList;
	for(let i = 0; i < msg["files"].length; ++i) {
	    let f = msg["files"][i];
	    l.push_back('/input/' + f.name);
	}

	FS.mkdir('/input');
	FS.mount(WORKERFS, { files: msg['files'] }, '/input');

	let res = m.process_images(msg["tileWidth"], msg["tileHeight"], l, progresscb);
	let resFile = FS.readFile(res);
	postMessage({'type': "result", 'name': res, 'data': resFile});
    };

    postMessage({'type': "ready"});
});
