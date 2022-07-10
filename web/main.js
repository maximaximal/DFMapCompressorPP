const dfmapcompressorpp = new Worker('worker.js');

function setReady() {
    console.log("Ready for processing!");
}

dfmapcompressorpp.onmessage = msg => {
    switch(msg.data.type) {
    case 'ready':
	setReady();
	break;
    }
}
