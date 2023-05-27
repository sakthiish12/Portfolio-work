//using System.Collections;
//using System.Collections.Generic;
//using UnityEngine;

//public class EnemyBehaviour : MonoBehaviour
//{
//    public Transform player;
//    public float moveSpeed = 5;
//    private Rigidbody2D rb;
//    private Vector2 movement;
//    public int enemyHealth = 5;
//    [SerializeField] private int enemyDamage = 2;
//    [SerializeField] private float enemyAttackSpeed = 1f;
//    private float canAttack;

//    public PlayerMovement playerHealth;
//    private void Start()
//    {
//        rb = this.GetComponent<Rigidbody2D>();
//    }
//    private void Update()
//    {
//        Vector3 direction = player.position - transform.position;
//        float angle = Mathf.Atan2(direction.y, direction.x) * Mathf.Rad2Deg;
//        direction.Normalize();
//        movement = direction;
//    }

//    private void FixedUpdate()
//    {
//        EnemyMove(movement);
//    }

//    public void EnemyTakeDamage(int damageAmount)
//    {
//        enemyHealth -= damageAmount;

//        if (enemyHealth == 0)
//        {
//            EnemyDie();
//        }
//    }
//    public void EnemyDie()
//    {
//        Destroy(gameObject);
//        Debug.Log("Enemy Dies");
//    }

//    void EnemyMove(Vector2 direction)
//    {
//       rb.MovePosition((Vector2)transform.position + (direction * moveSpeed * Time.deltaTime));
//    }


//    private void OnCollisionStay2D(Collision2D other)
//    {
//        if (other.gameObject.tag == "Player")
//        {
//            if (enemyAttackSpeed <= canAttack)
//            {
//                other.gameObject.GetComponent<PlayerMovement>().UpdateHealth(-enemyDamage);
//                playerHealth.healthBar.fillAmount -= 0.02f;
//                canAttack = 0f;
//            }
//            else
//            {
//                canAttack += Time.deltaTime;
//            }
//        }   
//    }
//}
