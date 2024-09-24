window.scrollTo({
    top: 0,
    behavior: 'smooth'
});

const filePathForm = document.getElementById("filePathForm")
const previewBlock = document.getElementById("previewBlock")
var pm3FilePath = null


const mainPage = document.getElementById('mainPage')
function setInputValue(name, value) {
    const input = mainPage.querySelector(`input[name="${name}"`)
    input.value = value;
}


function updatePreview() {
    previewBlock.innerHTML = `
        <img src="./img/preview.bmp" alt="Превью не найдено" height="240">
    `
}


filePathForm.addEventListener('submit', async (e) => {
    e.preventDefault()
    let path = filePathForm.querySelector('input[name="filepath"]').value
    path.replace(/["']/g, '')
    
    if (!path) {
        const errMsg = filePathForm.querySelector('.errorMsg')
        errMsg.textContent = 'Некоррекстный путь к файлу'
        errMsg.classList.add('active')
        return
    }

    const errMsg = filePathForm.querySelector('.errorMsg')
    errMsg.classList.add('active')
    const filesize = await getFileSize(path)
    if (filesize < 0) {
        errMsg.textContent = 'ты инвалид, введи норм путь к файлу'
        return
    }

    window.scrollBy({
        top: window.innerHeight,
        behavior: 'smooth'
    });

    setInputValue('filename', path.slice(2, 5))
    const filesizeStr = `${(filesize/1024/1024).toFixed(2)}mb`
    setInputValue('filesize', filesizeStr)

    const res = await setInputsValueCpp(path)
    if (!res) {
        setInputValue("filename", "eto polny jopa")
        return
    }
    updatePreview()
})









