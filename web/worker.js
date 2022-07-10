importScripts('./dfmapcompressorpp.js');

let progresscb = (p) => postMessage({'type': 'progress', 'progress': p});

Module().then(function(m) {
    onmessage = msg => {
	let l = new m.StringList;
	for(let i = 0; i < msg["files"].length; ++i)
	    l.push_back(msg["files"][i]);
	let res = m.process_images(msg["tilewidth"], msg["tileheight"], l, progresscb);
	postMessage({'type': "result", 'result': res});
    };

    postMessage({'type': "ready"});
});
