//using System.Collections;
//using System.Collections.Generic;
//using UnityEngine;
//using UnityEngine.UI;
//using UnityEditor.SceneManagement;
//using TMPro;

//public class PlayerMovement : MonoBehaviour
//{
//    public float Movespeed = 5f; //This sets the movespeed
//    public Rigidbody2D rb; // This calls our rigid body 2d
//    public Camera cam; //This sets our Camera
//    public int health = 0;
//    [SerializeField] private int currentHealth = 100;
//    public int maxHealth = 120;

//    public float Sprint = 5f; //sprint speed
//    public float currentSpeed;

//    Vector2 Movement;
//    Vector2 MousePos;

//    public Animator player_animator;

//    //for healthbar
//    public Image healthBar;
//    public TextMeshProUGUI health_zero;
//    public GameObject zero;

//    private void Start()
//    {
//        healthBar.fillAmount = currentHealth;
//        health = currentHealth;
//        health_zero.enabled = false;
//        zero.SetActive(false);
//        Time.timeScale = 1;
//    }

//    //this is for health
//    public void UpdateHealth(int mod)
//    {
//        health += mod;
//        if (health > currentHealth)
//        {
//            health = currentHealth;
//        }
//        else if (healthBar.fillAmount <= 0 || health <= 0)
//        {
//            health = 0;
//            healthBar.fillAmount = 0;
//            zero.SetActive(true);
//            health_zero.enabled = true;
//            Time.timeScale = 0;
//            //Destroy(gameObject);
//            Debug.Log("Player Dies");
//        }
//    }
//    // Update is called once per frame
//    void Update()
//    {
//        //for healthbar
//        if (healthBar.fillAmount > 0.5)
//        {
//            healthBar.color = Color.green;
//        }
//        else if (healthBar.fillAmount < 0.5 && healthBar.fillAmount > 0.3)
//        {
//            healthBar.color = Color.yellow;
//        }
//        else
//        {
//            healthBar.color = Color.red;
//        }

//        //for restarting
//        if (health_zero.isActiveAndEnabled == true) 
//        {
//            if (Input.GetKey(KeyCode.R) == true)
//            {
//                Time.timeScale = 1;
//                EditorSceneManager.LoadScene(0);
//            }
//        }
//        //this is for sprint
//        if (Input.GetKey(KeyCode.LeftShift))
//        {
//            currentSpeed = Movespeed + Sprint;
//        }
//        else
//        {
//            currentSpeed = Movespeed;
//        }

//        //this is for movement
//        Movement.x = Input.GetAxisRaw("Horizontal");
//        Movement.y = Input.GetAxisRaw("Vertical");

//        MousePos = cam.ScreenToWorldPoint(Input.mousePosition);

//        if (Movement.x > 0)
//        {
//            Debug.Log("right");
//            player_animator.SetBool("Right", true);
//            player_animator.SetBool("Left", false);
//            player_animator.SetBool("Up", false);
//            player_animator.SetBool("Down", false);
//        }
//        else if (Movement.x < 0)
//        {
//            Debug.Log("left");
//            player_animator.SetBool("Left", true);
//            player_animator.SetBool("Right", false);
//            player_animator.SetBool("Up", false);
//            player_animator.SetBool("Down", false);
//        }

//        if (Movement.y > 0)
//        {
//            Debug.Log("up");
//            player_animator.SetBool("Up", true);
//            player_animator.SetBool("Down", false);
//            player_animator.SetBool("Left", false);
//            player_animator.SetBool("Right", false);
//        }
//        else if (Movement.y < 0)
//        {
//            Debug.Log("down");
//            player_animator.SetBool("Down", true);
//            player_animator.SetBool("Up", false);
//            player_animator.SetBool("Left", false);
//            player_animator.SetBool("Right", false);
//        }
//    }

//    private void FixedUpdate()
//    {
//        rb.MovePosition(rb.position + Movement * currentSpeed * Time.fixedDeltaTime);

//        Vector2 lookDir = MousePos - rb.position;
//        float angle = Mathf.Atan2(lookDir.y, lookDir.x) * Mathf.Rad2Deg - 90f; //This sets the Z-Rotation


//    }
//}
