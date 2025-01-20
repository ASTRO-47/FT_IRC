# venv made an isolate enevirement for every project, so you can install the required dependencies, 
# if you tried to install the libraries globaly you may have a conflicts with diffrent projects needs diffrent libs.


from selenium import webdriver
from selenium.webdriver.chrome.options import Options
from selenium.webdriver.common.keys import Keys
from selenium.webdriver.common.by import By
from time import sleep
print("---------- first real tests-------by asoro----------")

chrome_options = Options()

chrome_options.add_experimental_option("detach", True)  
chrome_options.add_argument("--start-maximized")


driver = webdriver.Chrome()
driver.get("https://meta.intra.42.fr/clusters")
driver.maximize_window()
driver.find_element(By.ID, "username").send_keys("iez-zagh")

# input("hit enter to conitnue")
password_field = driver.find_element(By.ID, "password")
password_field.send_keys("Makeclean@123")

# Method 1: Send Enter key to the password field
password_field.send_keys(Keys.RETURN)
sleep(3)
# driver.find_element(By.LINK_TEXT, "e2").click()
try:
    driver.find_element(By.LINK_TEXT, "ael-kace")
    print("9assimo mlogi hhhh")
except:
    print("tfooo mamlogish")
driver.refresh()
input("hit enter to conitnue")
# sleep(3)
# driver.quit()