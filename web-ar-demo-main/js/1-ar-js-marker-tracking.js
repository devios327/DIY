AFRAME.registerComponent('scene-controller', {
    init: function () {
        this.el.addEventListener('loaded', function () {
            const barcodeMarker = document.querySelector('#barcode-marker')
            const modelSelector = document.querySelector('#model-selector')
            const models = barcodeMarker.children

            modelSelector.hidden = false

            for (const model of models) {
                if (modelSelector.value === model.dataset.listId) {
                    model.object3D.visible = true
                    break
                }
            }

            modelSelector.addEventListener('change', () => {
                for (const model of models) {
                    if (modelSelector.value === model.dataset.listId) {
                        model.object3D.visible = true
                    } else {
                        model.object3D.visible = false
                    }
                }
            })

            const camera = document.querySelector('[camera]');
            const marker = document.querySelector('a-marker');
            let check;

            marker.addEventListener('markerFound', () => {
                let cameraPosition = camera.object3D.position;
                let markerPosition = marker.object3D.position;
                let distance = cameraPosition.distanceTo(markerPosition)

                check = setInterval(() => {
                    cameraPosition = camera.object3D.position;
                    markerPosition = marker.object3D.position;
                    distance = cameraPosition.distanceTo(markerPosition)

                    // do what you want with the distance:
                    debug(distance);
                }, 100);
            });

            marker.addEventListener('markerLost', () => {
                clearInterval(check);
                //debug("");
            })
        })
    }
})

const debug = (str) =>{
    document.querySelector('.debug').innerHTML = str
}