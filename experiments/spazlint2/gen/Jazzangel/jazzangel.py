import requests
import difflib
import re

def compare_content_and_merge(content1, file2):
    with open(file2, 'r') as f2:
        text2 = f2.read()
    differ = difflib.unified_diff(content1.splitlines(keepends=True), text2.splitlines(keepends=True))
    merged_content = []
    for line in differ:
        if line.startswith(' '):
            merged_content.append(line[1:])
        elif line.startswith('+'):
            merged_content.append(line[1:])
    return ''.join(merged_content)

def patchJazzAngelJS(html_content):
    pattern = r'(</body>)'
    script_tag = '<script src="scripts/jazzangel.js"></script>\n'
    patched_html = re.sub(pattern, script_tag + r'\1', html_content, flags=re.DOTALL)
    return patched_html

def cleanHtml(html_content):
    pattern = r'''
        <meta.*?>\s*                    
        .*?                              
        <link.*?>                        
    '''
    cleaned_html = re.sub(pattern, r'<meta />\n<link rel="stylesheet" />', html_content, flags=re.DOTALL | re.VERBOSE)
    return cleaned_html

def downloadDocs():
    url = 'https://docs.jj2.plus/plus-angelscript.html'
    file_path = "data/blob.txt"
    response = requests.get(url)
    html_content = response.text
    merged_html = compare_content_and_merge(html_content, file_path)
    cleaned_html = cleanHtml(merged_html)
    patched_html = patchJazzAngelJS(cleaned_html)
    output_file = "plus-angelscript-merged.html"
    with open(output_file, "w", encoding='utf-8') as file:
        file.write(patched_html)    
    print(f"Merged and patched HTML saved to {output_file}")

downloadDocs()