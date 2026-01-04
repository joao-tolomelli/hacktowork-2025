const API_URL = "http://localhost:5000/";

export const fetchBanhos = async () => {
  try {
    const response = await fetch(`${API_URL}`);
    const data = await response.json();
    console.log(data);
    return data;
  } catch (error) {
    console.error("Error fetching chuveiro history:", error);
    throw error;
  }
};

export const fetchLatestBanho = async () => {
  try {
    const response = await fetch(`${API_URL}/latest`);
    const data = await response.json();
    return data;
  } catch (error) {
    console.error("Error fetching latest banho data:", error);
    throw error;
  }
};
